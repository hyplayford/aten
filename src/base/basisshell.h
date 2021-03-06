/*
	*** Basis Shell Definition
	*** src/base/basisshell.h
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

#ifndef ATEN_BASISSHELL_H
#define ATEN_BASISSHELL_H

#include <QList>
#include "base/atom.h"
#include "templates/list.h"
#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Basis Primitive
class BasisPrimitive : public ListItem<BasisPrimitive>
{
	public:
	// Constructor
	BasisPrimitive();

	/*
	 * Data
	 */
	private:
	// Basis function exponent
	double exponent_;
	// Basis function contraction coefficients
	QList<double> coefficients_;

	public:
	// Set basis function exponent
	void setExponent(double exponent);
	// Return basis function exponent
	double exponent() const;
	// Add contraction coefficient
	void addCoefficient(double cc);
	// Return number of defined coefficients
	int nCoefficients() const;
	// Return specified contraction coefficient
	double coefficient(int index);
};

// Basis Shell
class BasisShell : public ListItem<BasisShell>
{
	public:
	// Constructor
	BasisShell();
	// Function Type
	enum BasisShellType { NoType, SShellType, PShellType, DShellType, FShellType, GShellType, SPShellType, nBasisShellTypes };
	static BasisShellType basisShellType(QString s, bool reportError = false);
	static const char* basisShellType(BasisShellType bft);
	// Cartesian function names
	static int nCartesianFunctions(BasisShellType bft);
	static const char* cartesianFunction(BasisShellType bft, int index);
	// Spherical function names
	static int nSphericalFunctions(BasisShellType bft);
	static const char* sphericalFunction(BasisShellType bft, int index);


	/*
	 * Data
	 */
	private:
	// Index of associated atom
	int atomId_;
	// Shell type
	BasisShellType type_;
	// List of primitives
	List<BasisPrimitive> primitives_;

	public:
	// Set associated atom pointer
	void setAtomId(int id);
	// Return associated atom pointer
	int atomId() const;
	// Set basis function type
	void setType(BasisShellType bft);
	// Return basis function type
	BasisShellType type() const;
	// Add new primitive to shell
	BasisPrimitive* addPrimitive();
	// Return list of defined primitives
	BasisPrimitive* primitives();
	// Return n'th defined primitive
	BasisPrimitive* primitive(int id);
	// Return number of defined primitives
	int nPrimitives();
};

ATEN_END_NAMESPACE

#endif
