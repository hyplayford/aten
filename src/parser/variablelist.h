/*
	*** Variable List
	*** src/parser/variablelist.h
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

#ifndef ATEN_VARIABLELIST_H
#define ATEN_VARIABLELIST_H

#include "parser/variable.h"
#include "base/constants.h"
#include "templates/list.h"

// Forward Declarations
class IntegerVariable;
class NuCharacterVariable;
class DoubleVariable;

// Variable list
class VariableList
{
	/*
	// Variable List
	*/
	public:
	// Constructor / Destructor
	VariableList();

	private:
	// List of variables
	List<Variable> variables_;
	// List of constants
	List<Variable> constants_;
	// Create variable of specified type
	static Variable *makeVariable(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);
	// Create a new array variable in the list
	Variable *makeArray(VTypes::DataType type, const char *name, TreeNode *sizeexpr, TreeNode *initialValue = NULL);

	public:
	// Pass a newly-created variable / constant to the list for it to take ownership of
	void take(Variable *v);
	// Retrieve a named variable from the list
	Variable *find(const char *name);
	// Create a new variable in the list
	Variable *create(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);
	// Create a new variable (static function, so we don't take ownership of it).
	Variable *createFree(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);
	// Create a new array variable in the list
	Variable *createArray(VTypes::DataType type, const char *name, TreeNode *sizeexpr, TreeNode *initialValue = NULL);
	// Create a new array constant in the list
	Variable *createArrayConstant(VTypes::DataType type, int size);
	// Return the number of variables (not constants) contained in the list
	int nVariables();
	// Return first variable in the list
	Variable *first();
	// Reset all variable values
	bool initialise();
	// Clear all variables, expressions etc. stored in class
	void clear();
	// Print list of variables and their values
	void print();
};

#endif