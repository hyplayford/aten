/*
	*** ZMatrix Definition
	*** src/classes/zmatrix.cpp
	Copyright T. Youngs 2007-2010

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

// 'Prevent Windows collapsing in on itself' caveat
#define NOMINMAX

#include "classes/zmatrix.h"
#include "parser/double.h"
#include "model/model.h"
#include "base/sysfunc.h"

/*
// ZMatrix Element
*/

// Constructor
ZMatrixElement::ZMatrixElement()
{
	// Public variables
	prev = NULL;
	next = NULL;

	// Private variables
	parent_ = NULL;
	for (int i=0; i<4; ++i)
	{
		atoms_[i] = NULL;
		if (i<3)
		{
			values_[i] = NULL;
			negated_[i] = FALSE;
		}
	}
}

// Destructor
ZMatrixElement::~ZMatrixElement()
{
}

// Set parent
void ZMatrixElement::setParent(ZMatrix *parent)
{
	parent_ = parent;
}

// Set n'th atom datum
void ZMatrixElement::setAtom(int id, Atom *i)
{
	if ((id < 0) || (id > 3)) printf("Internal Error: Id for ZMatrixElement::setAtom is out of range (%i)\n", id);
	else atoms_[id] = i;
}

// Retrieve n'th atom datum
Atom *ZMatrixElement::atom(int id)
{
	if ((id < 0) || (id > 3)) printf("Internal Error: Id for ZMatrixElement::atom is out of range (%i)\n", id);
	else return atoms_[id];
	return NULL;
}

// Set n'th negate flag
void ZMatrixElement::setNegated(int id, bool b)
{
	if ((id < 0) || (id > 2)) printf("Internal Error: Id for ZMatrixElement::setNegate is out of range (%i)\n", id);
	else
	{
		negated_[id] = b;
		Model *m = parent_->parent();
		if (m != NULL) m->recalculateFromZMatrix();
	}
}

// Retrieve n'th negate flag
bool ZMatrixElement::negated(int id)
{
	if ((id < 0) || (id > 2)) printf("Internal Error: Id for ZMatrixElement::negate is out of range (%i)\n", id);
	else return negated_[id];
	return FALSE;
}
// Set distance (geometry variable 0)
void ZMatrixElement::setDistanceVariable(Variable *v)
{
	values_[0] = v;
}

// Retrieve distance variable (geometry variable 0)
Variable *ZMatrixElement::distanceVariable()
{
	return values_[0];
}

// Set distance variable name (geometry variable 0)
void ZMatrixElement::setDistanceName(const char *name)
{
	if (values_[0] == NULL) msg.print("Warning: No distance variable exists in ZMatrixElement, so can't set its name.\n");
	else values_[0]->setName(name);
}

// Retrieve distance variable name (geometry variable 0)
const char *ZMatrixElement::distanceName()
{
	static char name[32];
	name[0] = '\0';
	if (values_[0] == NULL) msg.print("Warning: No distance variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		if (negated_[0]) strcpy(name, "-");
		strcat(name, values_[0]->name());
	}
	return name;
}

// Set distance value
void ZMatrixElement::setDistance(double value)
{
	// Set variable value for distance, and recalculate model
	if (values_[0] == NULL) msg.print("Warning: No distance variable exists in ZMatrixElement to set.\n");
	else parent_->setVariable(values_[0], value);
}

// Retrieve distance (geometry variable 0)
double ZMatrixElement::distance()
{
	static ReturnValue rv;
	if (values_[0] == NULL) msg.print("Warning: No distance variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		values_[0]->execute(rv);
		return (negated_[0] ? -rv.asDouble() : rv.asDouble());
	}
	return 0.0;
}

// Set angle (geometry variable 1)
void ZMatrixElement::setAngleVariable(Variable *v)
{
	values_[1] = v;
}

// Retrieve angle variable (geometry variable 1)
Variable *ZMatrixElement::angleVariable()
{
	return values_[1];
}

// Set angle variable name (geometry variable 1)
void ZMatrixElement::setAngleName(const char *name)
{
	if (values_[1] == NULL) msg.print("Warning: No angle variable exists in ZMatrixElement, so can't set its name.\n");
	else values_[1]->setName(name);
}

// Retrieve angle variable name (geometry variable 0)
const char *ZMatrixElement::angleName()
{
	static char name[32];
	name[0] = '\0';
	if (values_[1] == NULL) msg.print("Warning: No angle variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		if (negated_[1]) strcpy(name, "-");
		strcat(name, values_[1]->name());
	}
	return name;
}

// Set angle value
void ZMatrixElement::setAngle(double value)
{
	// Set variable value for angle, and recalculate model
	if (values_[1] == NULL) msg.print("Warning: No angle variable exists in ZMatrixElement to set.\n");
	else parent_->setVariable(values_[1], value);
}

// Retrieve angle (geometry variable 1)
double ZMatrixElement::angle()
{
	static ReturnValue rv;
	if (values_[1] == NULL) msg.print("Warning: No angle variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		values_[1]->execute(rv);
		return (negated_[1] ? -rv.asDouble() : rv.asDouble());
	}
	return 0.0;
}

// Set torsion (geometry variable 2)
void ZMatrixElement::setTorsionVariable(Variable *v)
{
	values_[2] = v;
}

// Retrieve torsion variable (geometry variable 2)
Variable *ZMatrixElement::torsionVariable()
{
	return values_[2];
}

// Set torsion variable name (geometry variable 2)
void ZMatrixElement::setTorsionName(const char *name)
{
	if (values_[2] == NULL) msg.print("Warning: No torsion variable exists in ZMatrixElement, so can't set its name.\n");
	else values_[2]->setName(name);
}

// Retrieve torsion variable name (geometry variable 0)
const char *ZMatrixElement::torsionName()
{
	static char name[32];
	name[0] = '\0';
	if (values_[2] == NULL) msg.print("Warning: No torsion variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		if (negated_[2]) strcpy(name, "-");
		strcat(name, values_[2]->name());
	}
	return name;
}

// Set torsion value
void ZMatrixElement::setTorsion(double value)
{
	// Set variable value for torsion, and recalculate model
	if (values_[2] == NULL) msg.print("Warning: No torsion variable exists in ZMatrixElement to set.\n");
	else parent_->setVariable(values_[2], value);
}

// Retrieve torsion (geometry variable 2)
double ZMatrixElement::torsion()
{
	static ReturnValue rv;
	if (values_[2] == NULL) msg.print("Warning: No torsion variable exists in ZMatrixElement from which to return a value.\n");
	else
	{
		values_[2]->execute(rv);
		return (negated_[2] ? -rv.asDouble() : rv.asDouble());
	}
	return 0.0;
}

/*
// ZMatrix
*/

// Constructor
ZMatrix::ZMatrix()
{
	// Private variables
	parent_ = NULL;
}

// Destructor
ZMatrix::~ZMatrix()
{
}

// Return parent model
Model *ZMatrix::parent()
{
	return parent_;
}

// Return coordinate origin
Vec3<double> ZMatrix::origin()
{
	return origin_;
}

// Return total number of defined variables
int ZMatrix::nVariables()
{
	return (distances_.nVariables() + angles_.nVariables() + torsions_.nVariables());
}

// Add single definition to list
ZMatrixElement *ZMatrix::addElement(Reflist<Atom,int> &atoms)
{
	msg.enter("ZMatrix::addElement");
	int i;
	char name[32];
	DoubleVariable *v;
	// Create a new element structure, and store a maximum of 4 atoms from list in the element's array
	ZMatrixElement *zel = elements_.add();
	i = 0;
	for (Refitem<Atom,int> *ri = atoms.first(); ri != NULL; ri = ri->next)
	{
		zel->setAtom(i++, ri->item);
		if (i == 4) break;
	}
	// Variable 'i' now contains the number of atoms we have in this element
	if ((i < 1) || (i > 4))
	{
		printf("Internal Error: Attempted to create a ZMatrixElement with %i atoms\n", i);
		msg.exit("ZMatrix::addElement");
		return NULL;
	}
	// Set geometric values
	if (i > 1)
	{
		v = new DoubleVariable(parent_->distance(zel->atom(0), zel->atom(1)), FALSE);
		distances_.take(v);
		strcpy(name,"d");
		strcat(name, itoa(distances_.nVariables()));
		v->setName(name);
		zel->setDistanceVariable(v);
	}
	if (i > 2)
	{
		v = new DoubleVariable(parent_->angle(zel->atom(0), zel->atom(1), zel->atom(2)), FALSE);
		angles_.take(v);
		strcpy(name,"a");
		strcat(name, itoa(angles_.nVariables()));
		v->setName(name);
		zel->setAngleVariable(v);
	}
	if (i > 3)
	{
		v = new DoubleVariable(parent_->torsion(zel->atom(0), zel->atom(1), zel->atom(2), zel->atom(3)), FALSE);
		torsions_.take(v);
		strcpy(name,"t");
		strcat(name, itoa(torsions_.nVariables()));
		v->setName(name);
		zel->setTorsionVariable(v);
	}
	msg.exit("ZMatrix::addElement");
	return zel;
}

// Create (recursively) along bonds in the model wherever possible
void ZMatrix::createAlongBonds(Atom *target, Reflist<Atom,int> &atomlist)
{
	msg.enter("ZMatrix::createAlongBonds");
	// Add the current atom to the list and create an element for it
	atomlist.addStart(target);
	ZMatrixElement *zel = addElement(atomlist);
	// Mark this atom so it won't be added again
	parent_->selectAtom(target, TRUE);
	// Cycle over bonds, progressing along each connected atom
	Atom *i;
	for (Refitem<Bond,int> *ri = target->bonds(); ri != NULL; ri = ri->next)
	{
		i = ri->item->partner(target);
		if (i->isSelected(TRUE)) continue;
		createAlongBonds(i, atomlist);
	}
	msg.exit("ZMatrix::createAlongBonds");
}

// Create from specified model
void ZMatrix::create(Model *source, bool usebonds)
{
	msg.enter("ZMatrix::create");
	// Clear old data and set new target
	elements_.clear();
	distances_.clear();
	angles_.clear();
	torsions_.clear();
	parent_ = source;
	// List of previous atoms
	Reflist<Atom,int> atomlist;
	ZMatrixElement *zel;
	if (parent_->nAtoms() == 0)
	{
		msg.exit("ZMatrix::create");
		return;
	}
	// Create the elements
	origin_ = parent_->atoms()->r();
	if (TRUE)
	{
		parent_->selectNone(TRUE);
		createAlongBonds(parent_->atoms(), atomlist);
	}
	else
	{
		// Step through atoms in order, creating elements as we go...
		for (Atom *i = parent_->atoms(); i != NULL; i = i->next)
		{
			// Check current size of atomlist
			if (atomlist.nItems() == 4) atomlist.removeLast();
			// Add current atom to the reflist
			atomlist.addStart(i);
			// Create element
			zel = addElement(atomlist);
		}
	}
	msg.exit("ZMatrix::create");
}

// Return number of defined elements
int ZMatrix::nElements() const
{
	return elements_.nItems();
}

// Return specified element
ZMatrixElement *ZMatrix::elements() const
{
	return elements_.first();
}

// Return specified element
ZMatrixElement *ZMatrix::element(int index)
{
	return elements_[index];
}

// Return number of defined angle variables
int ZMatrix::nAngles()
{
	return angles_.nVariables();
}

// Return start of angles list
Variable *ZMatrix::angles()
{
	return angles_.variables();
}

// Return specified angle variable
Variable *ZMatrix::angle(int index)
{
	if ((index < 0) || (index >= angles_.nVariables())) printf("Array index %i is out of bounds for ZMatrix::angles\n", index);
	else return angles_.variable(index);
	return NULL;
}

// Return number of defined distance variables
int ZMatrix::nDistances()
{
	return distances_.nVariables();
}

// Return start of distances list
Variable *ZMatrix::distances()
{
	return distances_.variables();
}

// Return specified distance variable
Variable *ZMatrix::distance(int index)
{
	if ((index < 0) || (index >= distances_.nVariables())) printf("Array index %i is out of bounds for ZMatrix::distances\n", index);
	else return distances_.variable(index);
	return NULL;
}

// Return number of defined torsion variables
int ZMatrix::nTorsions()
{
	return torsions_.nVariables();
}

// Return start of torsions list
Variable *ZMatrix::torsions()
{
	return torsions_.variables();
}

// Return specified torsion variable
Variable *ZMatrix::torsion(int index)
{
	if ((index < 0) || (index >= torsions_.nVariables())) printf("Array index %i is out of bounds for ZMatrix::torsions\n", index);
	else return torsions_.variable(index);
	return NULL;
}

// Set variable value and update
void ZMatrix::setVariable(Variable *v, double value)
{
	msg.enter("ZMatrix::setVariable");
	// Check for NULL pointer
	if (v == NULL)
	{
		printf("Internal Error: NULL variable pointer passed to ZMatrix::setVariable\n");
		msg.exit("ZMatrix::setVariable");
		return;
	}
	// Set the new value of the specified variable
	ReturnValue newvalue(value);
	v->set( newvalue );
	updateModel();
	msg.exit("ZMatrix::setVariable");
}

// Recalculate atom positions in model
void ZMatrix::updateModel()
{
	// Reposition the model atoms to match
	parent_->recalculateFromZMatrix();
}
