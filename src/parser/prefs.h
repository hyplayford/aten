/*
	*** Prefs Variable
	*** src/parser/prefs.h
	Copyright T. Youngs 2007-2009

	This file is part of Aten.

	Prefs is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Prefs is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Prefs.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ATEN_PREFSVARIABLE_H
#define ATEN_PREFSVARIABLE_H

#include "parser/variable.h"
#include "parser/accessor.h"

// Forward Declarations
class TreeNode;

// Prefs Master Variable
class PrefsVariable : public Variable
{
	public:
	// Constructor / Destructor
	PrefsVariable();
	~PrefsVariable();

	/*
	// Set / Get
	*/
	public:
	// Return value of node
	bool execute(ReturnValue &rv);
	// Set from returnvalue node
	bool set(ReturnValue &rv);
	// Reset node
	void reset();

	/*
	// Variable Data
	*/
	private:
	// Print node contents
	void nodePrint(int offset, const char *prefix = "");

	/*
	// Access Data
	*/
	public:
	// Accessor list
	enum Accessors { EnergyUnit, nAccessors };
	// Search variable access list for provided accessor
	StepNode *findAccessor(const char *s, TreeNode *arrayindex);
	// Static function to search accessors
	static StepNode *accessorSearch(const char *s, TreeNode *arrayindex);
	// Retrieve desired value
	static bool retrieveAccessor(int i, ReturnValue &rv, bool hasarrayindex, int arrayIndex = -1);
	// Set desired value
	static bool setAccessor(int i, ReturnValue &sourcerv, ReturnValue &newvalue, bool hasarrayindex, int arrayIndex = -1);
	// Accessor data
	static Accessor accessorData[nAccessors];
};

#endif
