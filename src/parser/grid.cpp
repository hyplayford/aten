/*
	*** Grid Variable and Array
	*** src/parser/grid.cpp
	Copyright T. Youngs 2007-2011

	This file is part of Aten.

	Aten is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Aten is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Aten.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser/grid.h"
#include "parser/stepnode.h"
#include "model/model.h"
#include "classes/grid.h"
#include "base/constants.h"
#include "base/elements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
// Variable
*/

// Constructor
GridVariable::GridVariable(Grid *ptr, bool constant)
{
	// Private variables
	returnType_ = VTypes::GridData;
	readOnly_ = constant;
	pointerData_ = ptr;
}

// Destructor
GridVariable::~GridVariable()
{
}

/*
// Accessors
*/

// Accessor data
Accessor GridVariable::accessorData[GridVariable::nAccessors] = {
	{ "axes",			VTypes::CellData,	0, TRUE },
	{ "colour",			VTypes::DoubleData,	4, FALSE },
	{ "cutoff",			VTypes::DoubleData,	0, FALSE },
	{ "name",			VTypes::StringData,	0, FALSE },
	{ "nx",				VTypes::IntegerData,	0, TRUE },
	{ "ny",				VTypes::IntegerData,	0, TRUE },
	{ "nz",				VTypes::IntegerData,	0, TRUE },
	{ "origin", 			VTypes::VectorData,	0, FALSE },
	{ "outlinevolume",		VTypes::IntegerData,	0, FALSE },
	{ "periodic",			VTypes::IntegerData,	0, FALSE },
	{ "secondarycolour",		VTypes::DoubleData,	4, FALSE },
	{ "secondarycutoff",		VTypes::DoubleData,	0, FALSE },
	{ "secondaryuppercutoff",	VTypes::DoubleData,	0, FALSE },
	{ "shiftx",			VTypes::IntegerData,	0, FALSE },
	{ "shifty",			VTypes::IntegerData,	0, FALSE },
	{ "shiftz",			VTypes::IntegerData,	0, FALSE },
	{ "uppercutoff",		VTypes::DoubleData,	0, FALSE },
	{ "visible",			VTypes::IntegerData,	0, FALSE }
};

// Function data
FunctionAccessor GridVariable::functionData[GridVariable::nFunctions] = {
	{ "data",	VTypes::DoubleData,	"IIi",	"int i, int j, int k = -1" },
	{ "shift",	VTypes::NoData,		"IIIi",	"int dx, int dy, int dz, bool shiftAtoms = FALSE" }
};

// Search variable access list for provided accessor (call private static function)
StepNode *GridVariable::findAccessor(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	return GridVariable::accessorSearch(s, arrayindex, arglist);
}

// Private static function to search accessors
StepNode *GridVariable::accessorSearch(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	msg.enter("GridVariable::accessorSearch");
	StepNode *result = NULL;
	int i = 0;
	for (i = 0; i < nAccessors; i++) if (strcmp(accessorData[i].name,s) == 0) break;
	if (i == nAccessors)
	{
		// No accessor found - is it a function definition?
		for (i = 0; i < nFunctions; i++) if (strcmp(functionData[i].name,s) == 0) break;
		if (i == nFunctions)
		{
			msg.print("Error: Type 'grid&' has no member or function named '%s'.\n", s);
			printAccessors();
			msg.exit("GridVariable::accessorSearch");
			return NULL;
		}
		msg.print(Messenger::Parse, "FunctionAccessor match = %i (%s)\n", i, functionData[i].name);
		if (arrayindex != NULL)
		{
			msg.print("Error: Array index given to 'grid&' function '%s'.\n", s);
			msg.exit("GridVariable::accessorSearch");
			return NULL;
		}
		// Add and check supplied arguments...
		result = new StepNode(i, VTypes::GridData, functionData[i].returnType);
		result->addJoinedArguments(arglist);
		if (!result->checkArguments(functionData[i].arguments, functionData[i].name))
		{
			msg.print("Error: Syntax for 'grid&' function '%s' is '%s(%s)'.\n", functionData[i].name, functionData[i].name, functionData[i].argText );
			delete result;
			result = NULL;
		}
	}
	else
	{
		msg.print(Messenger::Parse, "Accessor match = %i (%s)\n", i, accessorData[i].name);
		// Were we given an array index when we didn't want one?
		if ((accessorData[i].arraySize == 0) && (arrayindex != NULL))
		{
			msg.print("Error: Irrelevant array index provided for member '%s'.\n", accessorData[i].name);
			result = NULL;
		}
		else result = new StepNode(i, VTypes::GridData, arrayindex, accessorData[i].returnType, accessorData[i].isReadOnly, accessorData[i].arraySize);
	}
	msg.exit("GridVariable::accessorSearch");
	return result;
}

// Retrieve desired value
bool GridVariable::retrieveAccessor(int i, ReturnValue &rv, bool hasArrayIndex, int arrayIndex)
{
	msg.enter("GridVariable::retrieveAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Grid type.\n", i);
		msg.exit("GridVariable::retrieveAccessor");
		return FALSE;
	}
	Accessors acc = (Accessors) i;
	// Check for correct lack/presence of array index given
	if ((accessorData[i].arraySize == 0) && hasArrayIndex)
	{
		msg.print("Error: Unnecessary array index provided for member '%s'.\n", accessorData[i].name);
		msg.exit("GridVariable::retrieveAccessor");
		return FALSE;
	}
	else if ((accessorData[i].arraySize > 0) && (hasArrayIndex))
	{
		if ((arrayIndex < 1) || (arrayIndex > accessorData[i].arraySize))
		{
			msg.print("Error: Array index out of bounds for member '%s' (%i, range is 1-%i).\n", accessorData[i].name, arrayIndex, accessorData[i].arraySize);
			msg.exit("GridVariable::retrieveAccessor");
			return FALSE;
		}
	}
	// Get current data from ReturnValue
	bool result = TRUE;
	Grid *ptr = (Grid*) rv.asPointer(VTypes::GridData, result);
	if (result && (ptr == NULL))
	{
		msg.print("Invalid (NULL) %s reference encountered.\n", VTypes::dataType(VTypes::GridData));
		result = FALSE;
	}
	if (result) switch (acc)
	{
		case (GridVariable::Axes):
			rv.set(VTypes::CellData, ptr->cell());
			break;
		case (GridVariable::Colour):
			if (hasArrayIndex) rv.set( ptr->primaryColour()[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->primaryColour(), 4);
			break;
		case (GridVariable::Cutoff):
			rv.set(ptr->lowerPrimaryCutoff());
			break;
		case (GridVariable::Name):
			rv.set(ptr->name());
			break;
		case (GridVariable::NX):
		case (GridVariable::NY):
		case (GridVariable::NZ):
			rv.set(ptr->nPoints().get(acc-GridVariable::NX));
			break;
		case (GridVariable::Origin):
			rv.set(ptr->origin());
			break;
		case (GridVariable::OutlineVolume):
			rv.set(ptr->outlineVolume());
			break;
		case (GridVariable::Periodic):
			rv.set(ptr->periodic());
			break;
		case (GridVariable::SecondaryColour):
			if (hasArrayIndex) rv.set( ptr->secondaryColour()[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->secondaryColour(), 4);
			break;
		case (GridVariable::SecondaryCutoff):
			rv.set(ptr->lowerSecondaryCutoff());
			break;
		case (GridVariable::SecondaryUpperCutoff):
			rv.set(ptr->upperSecondaryCutoff());
			break;
		case (GridVariable::ShiftX):
			rv.set(ptr->shift().x);
			break;
		case (GridVariable::ShiftY):
			rv.set(ptr->shift().y);
			break;
		case (GridVariable::ShiftZ):
			rv.set(ptr->shift().z);
			break;
		case (GridVariable::UpperCutoff):
			rv.set(ptr->upperPrimaryCutoff());
			break;
		case (GridVariable::Visible):
			rv.set(ptr->isVisible());
			break;
		default:
			printf("Internal Error: Access to member '%s' has not been defined in GridVariable.\n", accessorData[i].name);
			result = FALSE;
			break;
	}
	msg.exit("GridVariable::retrieveAccessor");
	return result;
}

// Set desired value
bool GridVariable::setAccessor(int i, ReturnValue &sourcerv, ReturnValue &newvalue, bool hasArrayIndex, int arrayIndex)
{
	msg.enter("GridVariable::setAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Grid type.\n", i);
		msg.exit("GridVariable::setAccessor");
		return FALSE;
	}
	Accessors acc = (Accessors) i;
	// Check for correct lack/presence of array index given to original accessor, and nature of new value
	bool result = TRUE;
	if (accessorData[i].arraySize != 0)
	{
		if (hasArrayIndex)
		{
			if ((accessorData[i].arraySize > 0) && ( (arrayIndex < 1) || (arrayIndex > accessorData[i].arraySize) ))
			{
				msg.print("Error: Array index provided for member '%s' is out of range (%i, range is 1-%i).\n", accessorData[i].name, arrayIndex, accessorData[i].arraySize);
				result = FALSE;
			}
			if (newvalue.arraySize() > 0)
			{
				msg.print("Error: An array can't be assigned to the single valued member '%s'.\n", accessorData[i].name);
				result = FALSE;
			}
		}
		else
		{
			if (newvalue.arraySize() > accessorData[i].arraySize)
			{
				msg.print("Error: The array being assigned to member '%s' is larger than the size of the desination array (%i cf. %i).\n", accessorData[i].name, newvalue.arraySize(), accessorData[i].arraySize);
				result = FALSE;
			}
		}
	}
	else
	{
		// This is not an array member, so cannot be assigned an array unless its a Vector
		if (newvalue.arraySize() != -1)
		{
			if (accessorData[i].returnType != VTypes::VectorData)
			{
				msg.print("Error: An array can't be assigned to the single valued member '%s'.\n", accessorData[i].name);
				result = FALSE;
			}
			else if ((newvalue.type() != VTypes::VectorData) && (newvalue.arraySize() != 3))
			{
				msg.print("Error: Only an array of size 3 can be assigned to a vector (member '%s').\n", accessorData[i].name);
				result = FALSE;
			}
		}
	}
	if (!result)
	{
		msg.exit("GridVariable::setAccessor");
		return FALSE;
	}
	// Get current data from ReturnValue
	Grid *ptr = (Grid*) sourcerv.asPointer(VTypes::GridData, result);
	int n;
	if (result && (ptr == NULL))
	{
		msg.print("Invalid (NULL) %s reference encountered.\n", VTypes::dataType(VTypes::GridData));
		result = FALSE;
	}
	if (result) switch (acc)
	{
		case (GridVariable::Colour):
			if (newvalue.arraySize() != -1) for (n=0; n<newvalue.arraySize(); ++n) ptr->primaryColour()[n] = newvalue.asDouble(n, result);
			else if (hasArrayIndex) ptr->primaryColour()[arrayIndex-1] = newvalue.asDouble(result);
			else for (n=0; n<4; ++n) ptr->primaryColour()[n] = newvalue.asDouble(result);
			break;
		case (GridVariable::Cutoff):
			ptr->setLowerPrimaryCutoff( newvalue.asDouble() );
			break;
		case (GridVariable::Name):
			ptr->setName( newvalue.asString() );
			break;
		case (GridVariable::Origin):
			ptr->setOrigin( newvalue.asVector() );
			break;
		case (GridVariable::OutlineVolume):
			ptr->setOutlineVolume( newvalue.asInteger() );
			break;
		case (GridVariable::Periodic):
			ptr->setPeriodic( newvalue.asInteger() );
			break;
		case (GridVariable::SecondaryColour):
			if (newvalue.arraySize() != -1) for (n=0; n<newvalue.arraySize(); ++n) ptr->secondaryColour()[n] = newvalue.asDouble(n, result);
			else if (hasArrayIndex) ptr->secondaryColour()[arrayIndex-1] = newvalue.asDouble(result);
			else for (n=0; n<4; ++n) ptr->secondaryColour()[n] = newvalue.asDouble(result);
			break;
		case (GridVariable::SecondaryCutoff):
			ptr->setLowerSecondaryCutoff( newvalue.asDouble() );
			break;
		case (GridVariable::SecondaryUpperCutoff):
			ptr->setUpperSecondaryCutoff( newvalue.asDouble() );
			break;
		case (GridVariable::ShiftX):
			ptr->setShift(0, newvalue.asInteger());
			break;
		case (GridVariable::ShiftY):
			ptr->setShift(1, newvalue.asInteger());
			break;
		case (GridVariable::ShiftZ):
			ptr->setShift(2, newvalue.asInteger());
			break;
		case (GridVariable::UpperCutoff):
			ptr->setUpperPrimaryCutoff( newvalue.asDouble() );
			break;
		case (GridVariable::Visible):
			ptr->setVisible( newvalue.asBool() );
			break;
		default:
			printf("GridVariable::setAccessor doesn't know how to use member '%s'.\n", accessorData[acc].name);
			result = FALSE;
			break;
	}
	msg.exit("GridVariable::setAccessor");
	return result;
}

// Perform desired function
bool GridVariable::performFunction(int i, ReturnValue &rv, TreeNode *node)
{
	msg.enter("GridVariable::performFunction");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nFunctions))
	{
		printf("Internal Error: FunctionAccessor id %i is out of range for Grid type.\n", i);
		msg.exit("GridVariable::performFunction");
		return FALSE;
	}
	// Get current data from ReturnValue
	bool result = TRUE;
	Grid *ptr = (Grid*) rv.asPointer(VTypes::GridData, result);
	int nx, ny, nz;
	if (result) switch (i)
	{
		case (GridVariable::Data):
			// Check type of grid data stored...
			switch (ptr->type())
			{
				case (Grid::RegularXYData):
					if (node->nArgs() == 3) msg.print("Warning: Third dimension given to 'data' function will be ignored...\n");
					nx = node->argi(0) - 1;
					ny = node->argi(1) - 1;
					if ((nx < 0) || (nx >= ptr->nPoints().x))
					{
						msg.print("Error: X value for grid (%i) is out of range (nx = %i)\n", nx+1, ptr->nPoints().x);
						result = FALSE;
					}
					else if ((ny < 0) || (ny >= ptr->nPoints().y))
					{
						msg.print("Error: Y value for grid (%i) is out of range (ny = %i)\n", ny+1, ptr->nPoints().y);
						result = FALSE;
					}
					else rv.set( ptr->data2d()[nx][ny] );
					break;
				case (Grid::RegularXYZData):
					if (node->nArgs() != 3)
					{
						msg.print("Error: Third dimension for 3D grid not provided in 'data' function.\n");
						result = FALSE;
						break;
					}
					nx = node->argi(0) - 1;
					ny = node->argi(1) - 1;
					nz = node->argi(2) - 1;
					if ((nx < 0) || (nx >= ptr->nPoints().x))
					{
						msg.print("Error: X value for grid (%i) is out of range (nx = %i)\n", nx+1, ptr->nPoints().x);
						result = FALSE;
					}
					else if ((ny < 0) || (ny >= ptr->nPoints().y))
					{
						msg.print("Error: Y value for grid (%i) is out of range (ny = %i)\n", ny+1, ptr->nPoints().y);
						result = FALSE;
					}
					else if ((nz < 0) || (nz >= ptr->nPoints().z))
					{
						msg.print("Error: Z value for grid (%i) is out of range (nz = %i)\n", nz+1, ptr->nPoints().z);
						result = FALSE;
					}
					else rv.set( ptr->data3d()[nx][ny][nz] );
					break;
				case (Grid::FreeXYZData):
					msg.print("Free (irregular) grid data cannot be accessed with the 'data' function.\n");
					result = FALSE;
					break;
			}
			break;
		case (GridVariable::Shift):
			ptr->setShift(ptr->shift().x+node->argi(0), ptr->shift().y+node->argi(1), ptr->shift().z+node->argi(2));
			if (node->argb(3))
			{
				Model *m = ptr->parent();
				// Determine shift amount...
				Vec3<double> vec;
				vec += ptr->axes().columnAsVec3(0) * node->argi(0);
				vec += ptr->axes().columnAsVec3(1) * node->argi(1);
				vec += ptr->axes().columnAsVec3(2) * node->argi(2);
				// Move atoms....
				m->beginUndoState("Shift atoms with grid");
				m->markAll();
				m->translateSelectionLocal(vec, TRUE);
				m->endUndoState();
			}
			break;
		default:
			printf("Internal Error: Access to function '%s' has not been defined in GridVariable.\n", functionData[i].name);
			result = FALSE;
			break;
	}
	msg.exit("GridVariable::performFunction");
	return result;
}

// Print valid accessors/functions
void GridVariable::printAccessors()
{
	if (GridVariable::nAccessors > 0)
	{
		msg.print("Valid accessors are:\n");
		for (int n=0; n<GridVariable::nAccessors; ++n) msg.print("%s%s%s", n == 0 ? " " : ", ", accessorData[n].name, accessorData[n].arraySize > 0 ? "[]" : "");
		msg.print("\n");
	}
	if ((GridVariable::nFunctions > 0) && (strcmp(functionData[0].name,".dummy") != 0))
	{
		msg.print("Valid functions are:\n");
		for (int n=0; n<GridVariable::nFunctions; ++n) msg.print("%s%s(%s)", n == 0 ? " " : ", ", functionData[n].name, functionData[n].argText);
		msg.print("\n");
	}
}

/*
// Variable Array
*/

// Constructor
GridArrayVariable::GridArrayVariable(TreeNode *sizeexpr, bool constant)
{
	// Private variables
	returnType_ = VTypes::GridData;
	pointerArrayData_ = NULL;
	arraySize_ = 0;
	nodeType_ = TreeNode::ArrayVarNode;
	readOnly_ = constant;
	arraySizeExpression_ = sizeexpr;
}

// Search variable access list for provided accessor
StepNode *GridArrayVariable::findAccessor(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	return GridVariable::accessorSearch(s, arrayindex, arglist);
}

