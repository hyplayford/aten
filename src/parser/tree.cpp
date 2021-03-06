/*
	*** Tree
	*** src/parser/tree.cpp
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

#include "parser/tree.h"
#include "parser/scopenode.h"
#include "parser/variablenode.h"
#include "parser/stepnode.h"
#include "parser/character.h"
#include "parser/element.h"
#include "parser/dialog.h"
#include "parser/newnode.h"
#include "base/prefs.h"
#include "main/aten.h"

ATEN_USING_NAMESPACE

// Static members
Aten* Tree::aten_ = NULL;

// Constructors
Tree::Tree() : ListItem<Tree>()
{
	// Private variables
	parent_ = NULL;
	acceptedFail_ = Commands::NoFunction;
	name_ = "unnamed";
	type_ = Tree::UnknownTree;
	returnType_ = VTypes::NoData;
	localScope_ = NULL;
	runCount_ = 0;
	createDefaultDialogFunction_ = NULL;
	defaultDialogCreated_ = false;

	// Initialise
	initialise();
}

// Destructor
Tree::~Tree()
{
	clear();
}

/*
 * Link to Aten
 */

// Set pointer to Aten
void Tree::setAten(Aten* aten)
{
	aten_ = aten;
}

/*
 * Character
 */

// Set parent
void Tree::setParent(Program* prog)
{
	parent_ = prog;
}

// Return parent
Program* Tree::parent() const
{
	return parent_;
}

// Set type
void Tree::setType(Tree::TreeType type)
{
	type_ = type;
}

// Return type
Tree::TreeType Tree::type() const
{
	return type_;
}

// Set name of tree
void Tree::setName(QString name)
{
	name_ = name;
}

// Return name of tree
QString Tree::name() const
{
	return name_;
}

// Set return type of tree
void Tree::setReturnType(VTypes::DataType dt)
{
	returnType_ = dt;
}

// Return return-type of tree
VTypes::DataType Tree::returnType() const
{
	return returnType_;
}

// Reset Tree, ready for new statement(s) to be added
void Tree::reset(bool clearVariables)
{
	Messenger::enter("Tree::reset");
	// Remove all nodes and statements except the first (which was the original root ScopeNode)
	TreeNode* rootnode = nodes_.first();
	nodes_.disown(rootnode);
	nodes_.clear();
	scopeStack_.clear();
	statements_.clear();
	// Cast rootnode into ScopeNode (if possible)
	ScopeNode* scope = NULL;
	if (rootnode->nodeType() == TreeNode::ScopedNode) scope = (ScopeNode*) rootnode;
	else printf("Internal Error: Failed to cast rootnode into a ScopeNode in Tree::reset().\n");
	
	// Re-own the root node and clear its variable list
	nodes_.own(rootnode);
	statements_.add(rootnode);
	if (scope)
	{
		scopeStack_.add( (ScopeNode*) rootnode);
		if (clearVariables)
		{
			scope->variables.clear();
			scope->createGlobalVariables();
		}
	}

	// Remove all local function nodes
	functions_.clear();
	
	Messenger::exit("Tree::reset");
}

// Finalise the tree contents, searching for specific functions etc.
bool Tree::finalise()
{
	Messenger::enter("Tree::finalise");

	// Does a createDialog function exist?
	createDefaultDialogFunction_ = findLocalFunction("createDefaultDialog");
	if (createDefaultDialogFunction_ != NULL)
	{
		// Does the function have the correct argument definition?
		if ((createDefaultDialogFunction_->nArgs() != 1) || (createDefaultDialogFunction_->args()->returnType() != VTypes::DialogData))
		{
			Messenger::print("Error: a 'createDefaultDialog' function exists, but has the wrong argument definition (it should take a single argument of type Dialog).");
			Messenger::exit("Tree::finalise");
			return false;
		}
		// Does the function have the correct return type?
		if (createDefaultDialogFunction_->returnType() != VTypes::NoData)
		{
			Messenger::print("Error: a 'createDefaultDialog' function exists, but has the wrong return type (which should be 'void').");
			Messenger::exit("Tree::finalise");
			return false;
		}
		Messenger::print(Messenger::Verbose, " --> Found 'createDefaultDialog' function in tree '%s'", qPrintable(name_));
	}
	
	// Call finalise on any child trees
	for (Tree* func = functions_.first(); func != NULL; func = func->next)
	{
		if (!func->finalise())
		{
			Messenger::exit("Tree::finalise");
			return false;
		}
	}

	Messenger::exit("Tree::finalise");
	return true;
}

/*
// Create / Execute
*/

// Set widget or global variable value
bool Tree::setAccessibleVariable(QString name, QString value)
{
	ReturnValue rv;
	bool result = false;

	// Check for a widget first, then for a global variable
	TreeGuiWidget* w = defaultDialog().findWidget(name);
	if (w != NULL)
	{
		Messenger::print(Messenger::Verbose, "Found default dialog widget '%s' - setting value to '%s'", qPrintable(name), qPrintable(value));
		result = defaultDialog().setWidgetValue(name, value);
	}
	else
	{
		Variable* var = globalVariables().find(name);
		if (var != NULL)
		{
			Messenger::print(Messenger::Verbose, "Found global variable '%s' in filter '%s' - setting value to '%s'", qPrintable(name), qPrintable(value));
			rv = value;
			result = var->set(rv);
		}
	}
	
	// Success?
	if (!result) Messenger::print("Error: Failed to find a widget (or a global variable) named '%s' in the current target.", qPrintable(name));
	return result;
}

// Clear contents of tree
void Tree::clear()
{
	nodes_.clear();
	statements_.clear();
	scopeStack_.clear();
	dialogs_.clear();
}

// (Re)Initialise Tree
void Tree::initialise()
{
	Messenger::enter("Tree::initialise");

	clear();

	// Add a dummy ScopeNode to contain the main variable list
	ScopeNode* root = new ScopeNode(Commands::NoFunction);
	root->setParent(this);
	root->createGlobalVariables();
	nodes_.own(root);
	scopeStack_.add(root);
	statements_.add(root);
	Messenger::exit("Tree::initialise");
}

// Set function for accepted fail
void Tree::setAcceptedFail(Commands::Function func)
{
	if ((acceptedFail_ != Commands::NoFunction) && (func != Commands::NoFunction)) printf("Warning: An acceptedFail command is already set...\n");
	acceptedFail_ = func;
}

// Clear accepted fail bit
Commands::Function Tree::acceptedFail() const
{
	return acceptedFail_;
}

// Execute tree
bool Tree::execute(ReturnValue& rv)
{
	Messenger::enter("Tree::execute");
	bool result = false;
	rv.reset();
	ElementMap::ZMapType zm = ElementMap::nZMapTypes;
	acceptedFail_ = Commands::NoFunction;

	++runCount_;

	for (RefListItem<TreeNode,int>* ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		Messenger::print(Messenger::Commands, "Executing tree statement %p...", ri->item);
// 		ri->item->nodePrint(1);
		result = ri->item->execute(rv);

		// Catch failures arising from 'return' statements
		if (acceptedFail_ == Commands::Return)
		{
			Messenger::print(Messenger::Parse, "Execution of tree ended early because we returned.");
			result = true;
			break;
		}
		else if (acceptedFail_ == Commands::Quit)
		{
			Messenger::print(Messenger::Parse, "Execution of tree ended early because we quit.");
			result = true;
			break;
		}
		else if (acceptedFail_ == Commands::Error)
		{
			result = false;
			break;
		}
		if (!result) break;
	}

	// Delete any temporary dialogs
	deleteDialogs();

	// Print some final verbose output
	Messenger::print(Messenger::Parse, "Final result from execution of tree '%s' (in Program '%s') is %s", qPrintable(name_), qPrintable(parent_->name()), qPrintable(rv.info()));
	if (!result) Messenger::print(Messenger::Parse, "Execution FAILED.");

	Messenger::exit("Tree::execute");
	return result;
}

// Return number of times tree has been run
int Tree::runCount()
{
	return runCount_;
}

// Print tree
void Tree::print()
{
	printf("Leaf Structure (%i statements):\n", statements_.nItems());
	int n=1;
	for (RefListItem<TreeNode,int>* ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		printf("-------------------------------------------------------------\n");
		printf("Statement %i:\n", n);
		printf("item pointer is %p\n", ri->item);
		ri->item->nodePrint(1);
		n ++;
	}
	printf("-------------------------------------------------------------\n");
}

/*
// Statements / Commands / Operators
*/

// Add a node representing a whole statement to the execution list
bool Tree::addStatement(TreeNode* leaf)
{
	if (leaf == NULL)
	{
		printf("Internal Error: NULL TreeNode passed to Tree::addStatement().\n");
		return false;
	}
	Messenger::print(Messenger::Parse, "Added statement node %p", leaf);
	leaf->setParent(this);
	statements_.add(leaf);
	return true;
}

// Add an operator to the Tree
TreeNode* Tree::addOperator(Commands::Function func, TreeNode* arg1, TreeNode* arg2, TreeNode* arg3)
{
	Messenger::enter("Tree::addOperator");
	// Check compatibility between supplied nodes and the operator, since we didn't check the types in the lexer.
	VTypes::DataType rtype;
	bool returnsarray;
	if (arg2 == NULL) rtype = checkUnaryOperatorTypes(func, arg1->returnType(), arg1->returnsArray(), returnsarray);
	else if (arg3 == NULL) rtype = checkBinaryOperatorTypes(func, arg1->returnType(), arg1->returnsArray(), arg2->returnType(), arg2->returnsArray(), returnsarray);
	else rtype = checkTernaryOperatorTypes(func, arg1->returnType(), arg1->returnsArray(), arg2->returnType(), arg2->returnsArray(), arg3->returnType(), arg3->returnsArray(), returnsarray); 
	if (rtype == VTypes::NoData) return NULL;
	
	// Create new command node
	CommandNode* leaf = new CommandNode(func);
	nodes_.own(leaf);
	Messenger::print(Messenger::Parse, "Added operator '%s' (%p)...", Commands::command(func), leaf);
	
	// Add arguments and set parent
	leaf->addArguments(1,arg1);
	leaf->setParent(this);
	if (arg2 != NULL) leaf->addArguments(1,arg2);
	if (arg3 != NULL) leaf->addArguments(1,arg3);
	leaf->setReturnType(rtype);
	leaf->setReturnsArray(returnsarray);
	
	Messenger::exit("Tree::addOperator");
	return leaf;
}

// Add a 'new' node to the Tree
TreeNode* Tree::addNew(VTypes::DataType type)
{
	Messenger::enter("Tree::addNew");
	
	// Check supplied type....
	if (type < VTypes::AtenData)
	{
		Messenger::print("Error : Plain datatypes cannot (and need not) be new'd in this way.");
		Messenger::exit("Tree::addNew");
		return NULL;
	}
	if (!VTypes::userCanCreate(type))
	{
		Messenger::print("Error : This datatype (%s) cannot be created in this way, or its usage is restricted by Aten.", VTypes::dataType(type));
		Messenger::exit("Tree::addNew");
		return NULL;
	}
	
	// Create the new node
	NewNode* node = new NewNode(type);

	Messenger::exit("Tree::addNew");
	return node;
}

// Add function-based leaf node to topmost branch on stack
TreeNode* Tree::addFunctionWithArglist(Commands::Function func, TreeNode* argList)
{
	Messenger::enter("Tree::addFunctionWithArglist");
	// Create new command node
	CommandNode* leaf = new CommandNode(func);
	nodes_.own(leaf);
	Messenger::print(Messenger::Parse, "Added function '%s' (%p)...", Commands::command(func), leaf);
	
	// Add argument list to node and set parent
	leaf->addJoinedArguments(argList);
	leaf->setParent(this);
	
	// Store the function's return type
	leaf->setReturnType(aten_->commandReturnType(func));
	
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(aten_->commandArguments(func), Commands::command(func)))
	{
		Messenger::print("Error: Function syntax is '%s(%s)'.", Commands::command(func), aten_->commandArgText(func));
		leaf = NULL;
	}
	else if (!leaf->prepFunction()) leaf = NULL;
	Messenger::exit("Tree::addFunctionWithArglist");
	return leaf;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
TreeNode* Tree::addFunction(Commands::Function func, TreeNode* a1, TreeNode* a2, TreeNode* a3, TreeNode* a4)
{
	Messenger::enter("Tree::addFunction");

	// Create new command node
	CommandNode* leaf = new CommandNode(func);
	nodes_.own(leaf);
	Messenger::print(Messenger::Parse, "Added function '%s' (%p)...", Commands::command(func), leaf);
	if (a1 != NULL) leaf->addArgument(a1);
	if (a2 != NULL) leaf->addArgument(a2);
	if (a3 != NULL) leaf->addArgument(a3);
	if (a4 != NULL) leaf->addArgument(a4);
	leaf->setParent(this);

	// Store the function's return type
	leaf->setReturnType(aten_->commandReturnType(func));

	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(aten_->commandArguments(func), Commands::command(func)))
	{
		Messenger::print("Error: Function syntax is '%s(%s)'.", Commands::command(func), aten_->commandArgText(func));
		leaf = NULL;
	}
	else if (!leaf->prepFunction()) leaf = NULL;

	Messenger::exit("Tree::addFunction");
	return leaf;
}

// Add user-defined function-based leaf node to topmost branch on stack
TreeNode* Tree::addUserFunction(Tree* func, TreeNode* argList)
{
	Messenger::enter("Tree::addUserFunction");
	// Create new command node
	UserCommandNode* leaf = new UserCommandNode(func);
	nodes_.own(leaf);
	Messenger::print(Messenger::Parse, "Added user function '%s' (%p)...", qPrintable(func->name()), leaf);

	// Add argument list to node and set parent
	leaf->addJoinedArguments(argList);
	leaf->setParent(this);

	// Store the function's return type
	leaf->setReturnType(func->returnType());

	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments()) leaf = NULL;
	Messenger::exit("Tree::addUserFunction");
	return leaf;
}

// Add a declaration list
TreeNode* Tree::addDeclarations(TreeNode* declist)
{
	Messenger::enter("Tree::addDeclarations");
	// Create new command node
	CommandNode* leaf = new CommandNode(Commands::Declarations);
	nodes_.own(leaf);
	Messenger::print(Messenger::Parse, "Added declarations node (%p)...", leaf);
	// Add argument list to node and set parent
	leaf->addJoinedArguments(declist);
	leaf->setParent(this);
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(aten_->commandArguments(Commands::Declarations), Commands::command(Commands::Declarations))) leaf = NULL;
	Messenger::exit("Tree::addDeclarations");
	return leaf;
}

// Link two arguments together with their member pointers
TreeNode* Tree::joinArguments(TreeNode* arg1, TreeNode* arg2)
{
	arg1->prevArgument = arg2;
	arg2->nextArgument = arg1;
	Messenger::print(Messenger::Parse, "Joining arguments %p and %p", arg1, arg2);
	return arg1;
}

// Join two commands together
TreeNode* Tree::joinCommands(TreeNode* node1, TreeNode* node2)
{
	CommandNode* leaf = new CommandNode(Commands::Joiner);
	nodes_.own(leaf);
	leaf->setParent(this);
	if (node1 != NULL) leaf->addArgument(node1);
	if (node2 != NULL) leaf->addArgument(node2);
	Messenger::print(Messenger::Parse, "Joined command nodes %p and %p (joiner node is %p)", node1, node2, leaf);
	return leaf;
}

// Add on a new scope to the stack
TreeNode* Tree::pushScope(Commands::Function func)
{
	ScopeNode* node = new ScopeNode();
	nodes_.own(node);
	scopeStack_.add(node,func);
	// The second scope node added to the tree will be the basic local one (in the case of a function)
	if (scopeStack_.nItems() == 2) localScope_ = node;
	Messenger::print(Messenger::Parse, "ScopeNode %p is pushed.", node);
	return node;
}

// Pop the topmost scope node
bool Tree::popScope()
{
	RefListItem<ScopeNode,int>* ri = scopeStack_.last();
	if (ri == NULL)
	{
		printf("Internal Error: No scoped node to pop from stack.\n");
		return false;
	}
	ScopeNode* temp = ri->item;
	scopeStack_.remove(ri);
	Messenger::print(Messenger::Parse, "ScopeNode %p is popped.", temp);
	return true;
}

/*
// Variables / Constants
*/

// Add constant value to tompost scope
TreeNode* Tree::addConstant(VTypes::DataType type, QString token)
{
	if (type == VTypes::IntegerData)
	{
		IntegerVariable* var = new IntegerVariable(token.toInt(), true);
		nodes_.own(var);
		return var;
	}
	else if (type == VTypes::DoubleData)
	{
		DoubleVariable* var = new DoubleVariable(token.toDouble(), true);
		nodes_.own(var);
		return var;
	}
	else if (type == VTypes::StringData)
	{
		StringVariable* var = new StringVariable(token, true);
		nodes_.own(var);
		return var;
	}
	else printf("Internal Error: Don't know how to create a constant of type '%s' for Tree.", VTypes::dataType(type));
	return NULL;
}

// Add integer constant
TreeNode* Tree::addConstant(int i)
{
	IntegerVariable* var = new IntegerVariable(i, true);
	nodes_.own(var);
	return var;
}

// Add double constant
TreeNode* Tree::addConstant(double d)
{
	DoubleVariable* var = new DoubleVariable(d, true);
	nodes_.own(var);
	return var;
}

// Add string constant
TreeNode* Tree::addConstant(QString s)
{
	StringVariable* var = new StringVariable(s, true);
	nodes_.own(var);
	return var;
}

// Add Element constant
TreeNode* Tree::addElementConstant(int el)
{
	ElementVariable* var;
	if ((el < 1) || (el > ElementMap::nElements())) var = new ElementVariable(NULL,true);
	else var = new ElementVariable(ElementMap::element(el), true);
	nodes_.own(var);
	return var;
}

// Add variable to topmost scope
TreeNode* Tree::addVariable(VTypes::DataType type, QString name, TreeNode* initialValue, bool global)
{
	if (global) Messenger::print(Messenger::Parse, "A new global variable '%s' is being created with type %s.", qPrintable(name), VTypes::dataType(type));
	else Messenger::print(Messenger::Parse, "A new variable '%s' is being created with type %s.", qPrintable(name), VTypes::dataType(type));

	// Get topmost scopenode or, if global variable, the parent programs global scopenode
	ScopeNode* scope;
	if (global) scope = &globalScope_;
	else
	{
		RefListItem<ScopeNode,int>* ri = scopeStack_.last();
		if (ri == NULL)
		{
			printf("Internal Error: No current scope in which to define variable '%s'.\n", qPrintable(name));
			return NULL;
		}
		scope = ri->item;
	}

	// Check initialvalue....
	if ((initialValue != NULL) && (type != VTypes::VectorData) && (type != VTypes::MatrixData))
	{
		if ((initialValue->nodeType() == TreeNode::ArrayVarNode) || (initialValue->nodeType() == TreeNode::ArrayConstantNode))
		{
			Messenger::print("Error: A non-array variable cannot be initialised from an array.");
			return NULL;
		}
	}

	// Create the supplied variable in the list of the topmost scope
	Variable* var = scope->variables.create(type, qPrintable(name), initialValue);
	if (!var)
	{
// 		printf("Failed to create variable '%s' in local scope.\n", name->get());
		return NULL;
	}
	Messenger::print(Messenger::Parse, "Created variable '%s' in scopenode %p", qPrintable(name), scope);
	return var;
}

// Add array variable to topmost ScopeNode using the most recently declared type
TreeNode* Tree::addArrayVariable(VTypes::DataType type, QString name, TreeNode* sizeexpr, TreeNode* initialvalue, bool global)
{
	if (global) Messenger::print(Messenger::Parse, "A new global array variable '%s' is being created with type %s.", qPrintable(name), VTypes::dataType(type));
	else Messenger::print(Messenger::Parse, "A new array variable '%s' is being created with type %s.", qPrintable(name), VTypes::dataType(type));
	// Get topmost scopenode or, if global variable, the parent programs global scopenode
	ScopeNode* scope;
	if (global) scope = &globalScope_;
	else
	{
		RefListItem<ScopeNode,int>* ri = scopeStack_.last();
		if (ri == NULL)
		{
			printf("Internal Error: No current scope in which to define array variable '%s'.\n", qPrintable(name));
			return NULL;
		}
		scope = ri->item;
	}
	// Create the supplied variable in the list of the topmost scope
	Variable* var = scope->variables.createArray(type, qPrintable(name), sizeexpr, initialvalue);
	if (!var)
	{
		printf("Internal Error: Failed to create array variable '%s' in local scope.\n", qPrintable(name));
		return NULL;
	}
	Messenger::print(Messenger::Parse, "Created array variable '%s' in scopenode %p", qPrintable(name), scope);
	return var;
}

// Add constant vector
TreeNode* Tree::addArrayConstant(TreeNode* values)
{
	RefListItem<ScopeNode,int>* ri = scopeStack_.last();
	// Determine numbers of each type in array
	TreeNode* first;
	bool badData = false;
	int nints = 0, ndoubles = 0, nstrings = 0, npointers = 0, nvalues = 0;
	VTypes::DataType dt = VTypes::NoData;
	for (first = values; first != NULL; first = first->prevArgument)
	{
		++nvalues;
		switch (first->returnType())
		{
			case (VTypes::IntegerData):
				++nints;
				if (nstrings+npointers > 0) badData = true;
				break;
			case (VTypes::DoubleData):
				++ndoubles;
				if (nstrings+npointers > 0) badData = true;
				break;
			case (VTypes::StringData):
				++nstrings;
				if (nints+ndoubles+npointers > 0) badData = true;
				break;
			default:
				++npointers;
				if (nints+ndoubles+nstrings > 0) badData = true;
				if ((dt != VTypes::NoData) && (dt != first->returnType())) badData = true;
				dt = first->returnType();
				break;
		}
		if (badData) break;
		if (first->prevArgument == NULL) break;
	}

	// Check for bad data in array specification
	if (badData)
	{
		Messenger::print("Error: Incompatible mixture of data types found in array declaration.");
		return NULL;
	}

	// Type of array will be 'highest' type that we found
	if (npointers > 0) dt = values->returnType();
	else if (nstrings > 0) dt = VTypes::StringData;
	else if (ndoubles > 0) dt = VTypes::DoubleData;
	else dt = VTypes::IntegerData;
	Variable* var = ri->item->variables.createArrayConstant(dt, nvalues);
	var->setParent(this);
	var->addJoinedArguments(values);
	nodes_.own(var);
	return var;
}

// Search for variable in current scope
Variable* Tree::findLocalVariable(QString name, int& scopelevel)
{
	Variable* result = NULL;
	scopelevel = 0;
	Messenger::print(Messenger::Parse, "Searching scope for variable '%s'...", qPrintable(name));
	// Search the current ScopeNode list for the variable name requested
	for (RefListItem<ScopeNode,int>* ri = scopeStack_.last(); ri != NULL; ri = ri->prev)
	{
		Messenger::print(Messenger::Parse," ... scopenode %p...", ri->item);
		result = ri->item->variables.find(name);
		if (result != NULL)
		{
			Messenger::print(Messenger::Parse, "...variable '%s' found at a scope level of %i.", qPrintable(name), scopelevel);
			return result;
		}
		--scopelevel;
	}
	
// 	// Didn't find the variable in any local scope - check global scope
// 	result = globalScope_.variables.find(name);
// 	if (result != NULL)
// 	{
// 		scopelevel = 1;
// 		Messenger::print(Messenger::Parse, "...variable '%s' found at global scope.", name);
// 		return result;
// 	}
	
// 	// Not in global scope - was it passed as a CLI value?
// 	result = aten_->findPassedValue(name);
// 	if (result != NULL)
// 	{
// 		scopelevel = 2;
// 		Messenger::print(Messenger::Parse, "...variable '%s' found as a passed value.", name);
// 		return result;
// 	}
	
	Messenger::print(Messenger::Parse, "...no variable '%s' found in any scope.", qPrintable(name));
	return NULL;
}

// Wrap named variable (and array index)
TreeNode* Tree::wrapVariable(Variable* var, TreeNode* arrayIndex)
{
	// If an array index was given, check that the target variable is actually an array....
	if (arrayIndex && (var->nodeType() != TreeNode::ArrayVarNode))
	{
		Messenger::print("Error: Array index given to variable '%s', but it is not an array.", qPrintable(var->name()));
		return NULL;
	}
	VariableNode* vnode = new VariableNode(var);
	nodes_.own(vnode);
	vnode->setArrayIndex(arrayIndex);
	if ((arrayIndex == NULL) && (var->nodeType() == TreeNode::ArrayVarNode)) vnode->setReturnsArray(true);
	vnode->setParent(this);
	return vnode;
}

// Return local scope's variable list
const VariableList& Tree::localVariables() const
{
	return localScope_->variables;
}

// Return global scope's variable list
const VariableList& Tree::globalVariables() const
{
	return globalScope_.variables;
}

/*
 * Paths
 */

// Create a new path on the stack
TreeNode* Tree::createPath(TreeNode* node)
{
	Messenger::enter("Tree::createPath");
	VariableNode* vnode = (VariableNode*) node;
	pathStack_.add(vnode, vnode);
	Messenger::print(Messenger::Parse, "A new path has been started, beginning from variable '%s'.", qPrintable(vnode->name()));
	Messenger::exit("Tree::createPath");
	return vnode;
}

// Pop topmost path from stack
TreeNode* Tree::finalisePath()
{
	Messenger::enter("Tree::finalisePath");
	// Finalise the path before we remove it
	RefListItem<VariableNode,TreeNode*>* ri = pathStack_.last();
	if (ri == NULL)
	{
		Messenger::print("Internal Error: No path on stack to finalise.");
		return NULL;
	}
	ri->item->finalisePath();
	TreeNode* result = ri->item;
	Messenger::print(Messenger::Parse, "Path beginning from variable '%s' has been finalised.", qPrintable(ri->item->name()));
	pathStack_.remove(ri);
	Messenger::exit("Tree::finalisePath");
	return result;
}

// Expand the topmost path on the stack
bool Tree::expandPath(QString name, TreeNode* arrayIndex, TreeNode* argList)
{
	Messenger::enter("Tree::expandPath");
	// Check if both an arrayIndex and an argList were supplied, which is invalid
	if ((arrayIndex != NULL) && (argList != NULL))
	{
		printf("Internal Error: Both an array index and an argument list were provided for a path step.\n");
		return false;
	}
	// Get last item on path stack
	RefListItem<VariableNode,TreeNode*>* ri = pathStack_.last();
	if (ri == NULL)
	{
		printf("Internal Error: No path on stack to expand with accessor '%s'.\n", qPrintable(name));
		return false;
	}
	Messenger::print(Messenger::Parse,"Tree is evaluating accessor '%s' as step %i from the basenode '%s'...", qPrintable(name), ri->item->nArgs()+1, qPrintable(ri->item->name()));
	
	// If the last step was an array and an array index was not give, we complain!
	if (ri->item != ri->data)
	{
		StepNode* laststep = (StepNode*) ri->data;
		if ((laststep->arraySize() > 0) && (laststep->arrayIndex() == NULL))
		{
			Messenger::print("Previous step in path requires an array index to be specified.");
			Messenger::exit("Tree::expandPath");
			return false;
		}
	}
	
	// Find next step accessor
	StepNode* result = ri->data->findAccessor(name, arrayIndex, argList);
	// If we found a valid accessor, update the pathstack entry
	if (result)
	{
		Messenger::print(Messenger::Parse,"...OK - matching accessor found: return type is %s", VTypes::dataType(result->returnType()));
		ri->data = (TreeNode*) result;
		nodes_.own(result);
		result->setParent(this);
		// Finalise the path before we remove it
		RefListItem<VariableNode,TreeNode*>* ri = pathStack_.last();
		if (ri == NULL)
		{
			Messenger::print("Internal Error: No path on stack to expand!");
			Messenger::exit("Tree::expandPath");
			return false;
		}
		ri->item->addArgument(result);
	}
// 	else Messenger::print("Error: Object of type '%s' has no matching accessor for '%s'.", VTypes::dataType(ri->data->returnType()), name->get());
	Messenger::exit("Tree::expandPath");
	return result;
}

// Return number of arguments defined (for function)
int Tree::nArgs() const
{
	return arguments_.nItems();
}

// Return first argument defined (for function)
TreeNode* Tree::args() const
{
	return arguments_.first();
}

// Return first in stack of scopenodes
RefListItem<ScopeNode,int>* Tree::scopeNodes()
{
	return scopeStack_.first();
}

/*
 * Local Functions
 */

// Search for existing local function
Tree* Tree::findLocalFunction(QString functionName) const
{
	Tree* function;
	for (function = functions_.first(); function != NULL; function = function ->next) if (functionName == function->name()) break;
	return function;
}

// Add new local function
Tree* Tree::addLocalFunction(QString functionName)
{
	Tree* function = functions_.add();
	function->setName(functionName);
	function->setType(Tree::FunctionTree);
	function->setParent(parent_);
	return function;
}

// Add arguments to local function (topmost in stack)
bool Tree::addLocalFunctionArguments(TreeNode* argList)
{
	if (type_ != Tree::FunctionTree)
	{
		printf("Internal Error: Target tree is not a function.\n");
		return false;
	}
	TreeNode* first, *node;
	VariableNode* vnode;
	// Rewind to head of arguments list
	for (first = argList; first != NULL; first = first->prevArgument) if (first->prevArgument == NULL) break;
	// Wrap the argument variables supplied
	for (node = first; node != NULL; node = node->nextArgument)
	{
		Variable* var = (Variable*) node;
		vnode = new VariableNode(var);
		arguments_.own(vnode);
		vnode->setParent(this);
	}
	return true;
}

/*
 * Qt/CLI GUI Definition
 */

// Return default dialog structure
TreeGui& Tree::defaultDialog()
{
	// Run the stored 'createDefaultDialog' function if it hasn't already been done
	if (!defaultDialogCreated_)
	{
		if (createDefaultDialogFunction_ == NULL) defaultDialogCreated_ = true;
		else
		{
			UserCommandNode createFunc;
			DialogVariable dialogVar(&defaultDialog_);
			createFunc.setParent(this);
			createFunc.addArgument(&dialogVar);
			createFunc.setFunction(createDefaultDialogFunction_);
			ReturnValue rv;
			defaultDialogCreated_ = createFunc.execute(rv);
		}
	}
	return defaultDialog_;
}

// Create and return new, temporary dialog
TreeGui* Tree::createDialog(QString title)
{
	TreeGui* dialog = new TreeGui;
	dialogs_.own(dialog);
	dialog->setProperty(TreeGuiWidgetEvent::TextProperty, title);
	return dialog;
}

// Delete temporary dialogs
void Tree::deleteDialogs()
{
	while (dialogs_.first() != NULL)
	{
		dialogs_.removeLast();
	}
}
