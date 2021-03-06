/*
	*** Atomic bond
	*** src/base/bond.h
	Copyright T. Youngs 2007-2017

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

#ifndef ATEN_BOND_H
#define ATEN_BOND_H

#include "templates/list.h"
#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Forward Declarations (Aten)
class Atom;

// Basic Bond Definition
class Bond : public ListItem<Bond>
{
	public:
	// Constructor
	Bond();
	// Bond types
	enum BondType { Any = 0, Single, Double, Triple, Aromatic, nBondTypes };
	// Bond type from text
	static BondType bondType(QString s, bool reportError = false);
	// Bond type from order
	static BondType bondType(double order);
	// BondType Keywords
	static const char* bondType(BondType);
	// Increase simple BondType (instead of defining postfix increment)
	static BondType increase(BondType);
	// Decrease simple BondType (instead of defining postfix increment)
	static BondType decrease(BondType);

	private:
	// Pointers to both atoms involved in the bond
	Atom* atomI_, *atomJ_;
	// Bond type
	BondType type_;

	public:
	// Set atoms for bond
	void setAtoms(Atom* i, Atom* j);
	// Set atom I for bond
	void setAtomI(Atom* i);
	// Return first atom in bond
	Atom* atomI();
	// Set atom I for bond
	void setAtomJ(Atom* j);
	// Return second atom in bond
	Atom* atomJ();
	// Set bond type
	void setType(BondType bt);
	// Return type of bond
	BondType type() const;
	// Return real-valued bond order
	double order() const;
	// Return real-valued bond order (static function)
	static double order(BondType bt);
	// Returns the partner of the specified atom in the bond structure
	Atom* partner(Atom* i);
	// Return bond type that best satisfies the involved atoms valencies
	Bond::BondType augmented() const;
};

ATEN_END_NAMESPACE

#endif
