/*
	*** Variable command functions
	*** src/command/variables.cpp
	Copyright T. Youngs 2007-2009

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

#include "variables/accesspath.h"
#include "command/commandlist.h"
#include "model/model.h"
#include "base/elements.h"
#include "base/spacegroup.h"
#include "base/sysfunc.h"
#include "base/pattern.h"
#include "classes/grid.h"
#include "ff/forcefield.h"
#include "classes/forcefieldatom.h"
#include "classes/forcefieldbound.h"

// Get part of string before specified character
int Command::function_CA_AFTERCHAR(CommandNode *&c, Bundle &obj)
{
	c->arg(2)->set( afterChar(c->argc(0), c->argc(1)[0]) );
	return Command::Success;
}

// Get part of string before specified character
int Command::function_CA_BEFORECHAR(CommandNode *&c, Bundle &obj)
{
	c->arg(2)->set( beforeChar(c->argc(0), c->argc(1)[0]) );
	return Command::Success;
}

// Decrease variable by 1
int Command::function_CA_DECREASE(CommandNode *&c, Bundle &obj)
{
	c->arg(0)->step(-1);
	return Command::Success;
}

// Increase variable
int Command::function_CA_INCREASE(CommandNode *&c, Bundle &obj)
{
	c->arg(0)->step(1);
	return Command::Success;
}

// Set non-pointer or non-character variable to value, variable, or expression
int Command::function_CA_LET(CommandNode *&c, Bundle &obj)
{
	// Our action depends on the type of the variable being assigned to
	VTypes::DataType type1 = c->argt(0);
	VTypes::DataType type2 = c->argt(2);
	// Integer and real variables may only be set from character, integer, real, or expression variables
	switch (type1)
	{
		case (VTypes::IntegerData):
			if (type2 > VTypes::RealData)
			{
				msg.print( "Cannot set integer variable '%s' from pointer variable '%s'.\n", c->arg(0)->name(), c->arg(2)->name());
				return Command::Fail;
			}
			break;
		case (VTypes::RealData):
			if (type2 > VTypes::RealData)
			{
				msg.print( "Cannot set real variable '%s' from pointer variable '%s'.\n", c->arg(0)->name(), c->arg(2)->name());
				return Command::Fail;
			}
			break;
		// All other types are pointers - the second argument must also then be a pointer
		default:
			printf("CA_LET doesn't know how to handle variable assignments of type '%s'\n", VTypes::dataType(c->argt(0)));
			return Command::Fail;
			break;
	}
	// Create undostate if a valid model is available
	if (obj.rs != NULL) obj.rs->beginUndoState("Set vector quantity\n");
	// Perform assignment operation requested
	switch (c->argi(1))
	{
		case (AssignOps::Equals):
			c->arg(0)->set( type1 == VTypes::IntegerData ? c->arg(2)->asInteger() : c->arg(2)->asDouble() );
			break;
		case (AssignOps::MinusEquals):
			c->arg(0)->set( type1 == VTypes::IntegerData ? c->arg(0)->asInteger() - c->arg(2)->asInteger() : c->arg(0)->asDouble() - c->arg(2)->asDouble() );
			break;
		case (AssignOps::PlusEquals):
			if (type1 == VTypes::IntegerData) c->arg(0)->set( c->arg(0)->asInteger() + c->arg(2)->asInteger() );
			else c->arg(0)->set( c->arg(0)->asDouble() + c->arg(2)->asDouble() );
			break;
		case (AssignOps::DivideEquals):
			c->arg(0)->set( type1 == VTypes::IntegerData ? c->arg(0)->asInteger() / c->arg(2)->asInteger() : c->arg(0)->asDouble() / c->arg(2)->asDouble() );
			break;
		case (AssignOps::MultiplyEquals):
			c->arg(0)->set( type1 == VTypes::IntegerData ? c->arg(0)->asInteger() * c->arg(2)->asInteger() : c->arg(0)->asDouble() * c->arg(2)->asDouble() );
			break;
	}
	if (obj.rs != NULL) obj.rs->endUndoState();
	return Command::Success;
}

// Set vector variable
int Command::function_CA_LETVECTOR(CommandNode *&c, Bundle &obj)
{
	// Our action depends on the type of the variable being assigned to
	VTypes::DataType type2 = c->argt(2);
	VTypes::DataType type1 = c->argt(0);
	// Failsafe check for type of lhs argument
	if (type1 != VTypes::VectorData)
	{
		msg.print( "'letvector' cannot be used to assign values to a variable of type '%s'\n", VTypes::dataType(type1));
		return Command::Fail;
	}
	// Get LHS value
	Vec3<double> v1 = c->arg(0)->asVector();
	// Vector variables may only be set from character, integer, real, vector, or constvector variables
	if ((type2 >= VTypes::AtomData) && (type2 < VTypes::VectorData))
	{
		msg.print( "Cannot set vector variable '%s' from pointer variable '%s'.\n", c->arg(0)->name(), c->arg(2)->name());
		return Command::Fail;
	}
	// Get RHS value
	Vec3<double> v2;
	bool rhsvector = FALSE;
	if ((type2 == VTypes::VectorData) || (type2 == VTypes::ConstVectorData)) rhsvector = TRUE;
	if (rhsvector) v2 = c->arg(2)->asVector();
	// Create undostate if a valid model is available
	if (obj.rs != NULL) obj.rs->beginUndoState("Set vector quantity\n");
	// Perform assignment operation requested
	switch (c->argi(1))
	{
		case (AssignOps::Equals):
			if (rhsvector) c->arg(0)->set(v2);
			else c->arg(0)->set(c->arg(2)->asDouble());
			break;
		case (AssignOps::MinusEquals):
			if (rhsvector) c->arg(0)->set( v1 - v2 );
			else c->arg(0)->set( v1 - c->arg(2)->asDouble() );
			break;
		case (AssignOps::PlusEquals):
			if (rhsvector) c->arg(0)->set( v1 + v2 );
			else c->arg(0)->set( v1 + c->arg(2)->asDouble() );
			break;
		case (AssignOps::DivideEquals):
			if (rhsvector) c->arg(0)->set( v1 / v2 );
			else c->arg(0)->set( v1 / c->arg(2)->asDouble() );
			break;
		case (AssignOps::MultiplyEquals):
			if (rhsvector) c->arg(0)->set( v1 * v2 );
			else c->arg(0)->set( v1 * c->arg(2)->asDouble() );
			break;
	}
	if (obj.rs != NULL) obj.rs->endUndoState();
	return Command::Success;
}

// Assign string/variable to character variable only
int Command::function_CA_LETCHAR(CommandNode *&c, Bundle &obj)
{
	// Our action depends on the operator provided which we cast from the second argument
	Dnchar tempstring;
	switch (c->argi(1))
	{
		// Straight assigment
		case (AssignOps::Equals):
			c->arg(0)->set(c->argc(2));
			break;
		// Concatenation
		case (AssignOps::PlusEquals):
			tempstring = c->argc(0);
			tempstring.cat(c->argc(2));
			c->arg(0)->set(tempstring.get());
			break;
		default:
			printf("Operator given to CA_LETCHAR (%i) that we don't know how to handle.\n", c->argi(1));
			break;
	}
	return Command::Success;
}

// Assign pointer variable to another pointer variable
int Command::function_CA_LETPTR(CommandNode *&c, Bundle &obj)
{
	if (c->argt(0) != c->argt(2))
	{
		msg.print( "Incompatible pointer types for variable assignment of contents of '%s' to '%s'.\n", c->arg(0)->name(), c->arg(2)->name());
		return Command::Fail;
	}
	else c->arg(0)->set(c->arg(2)->asPointer(c->argt(0)), c->argt(0));
	return Command::Success;
}

// Normalise 3-vector
int Command::function_CA_NORMALISE(CommandNode *&c, Bundle &obj)
{
	Vec3<double> v = c->arg3d(0);
	v.normalise();
	c->arg(0)->set(v.x);
	c->arg(1)->set(v.y);
	c->arg(2)->set(v.z);
	return Command::Success;
}

// Strip characters from supplied string
int Command::function_CA_STRIPCHARS(CommandNode *&c, Bundle &obj)
{
	c->arg(0)->set( stripChars(c->argc(0), c->argc(1)) );
	return Command::Success;
}