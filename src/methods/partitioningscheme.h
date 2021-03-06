/*
	*** Partitioning Scheme
	*** src/methods/partitioningscheme.h
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

#ifndef ATEN_PARTITIONINGSCHEME_H
#define ATEN_PARTITIONINGSCHEME_H

#include "base/grid.h"
#include "parser/program.h"
#include "parser/usercommandnode.h"
#include "parser/double.h"
#include "parser/integer.h"
#include "methods/disorderdata.h"
#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Partitioning Scheme for Disordered Builder
class PartitioningScheme : public ListItem<PartitioningScheme>
{
	public:
	// Constructor / Destructor
	PartitioningScheme();
	~PartitioningScheme();


	/*
	 * Function Data
	 */
	private:
	// Program defining scheme, variables and functions
	Program schemeDefinition_;
	// Name of scheme (retrieved fron name() function)
	QString name_;
	// Description of scheme (retrieved from description() function)
	QString description_;
	// Pointer to partition() function
	Tree* partitionFunction_;
	// Pointer to partitionName() function
	Tree* partitionNameFunction_;
	// Pointer to partitionOptions() function
	Tree* partitionOptionsFunction_;
	
	public:
	// Return Program structure
	Program &schemeDefinition();
	// Setup scheme information from generated program structure
	bool initialiseFromProgram();
	// Setup scheme information manually (for absolute grid data)
	void initialiseAbsolute(QString name, QString description);
	// Set name and description of scheme manually
	void setName(QString name, QString description = QString());
	// Return name of partitioning scheme
	QString name();
	// Return description of partitioning scheme
	QString description();
	// Set named variable in partitionFunction_'s local scope
	bool setVariable(QString name, QString value);


	/*
	 * Partition Data
	 */
	private:
	// Log counter for gridsize / variable changes
	int changeLog_;
	// Flag specifying whether contained partition and grid data is static
	bool staticData_;
	// List of all possible partitions in scheme
	List<PartitionData> partitions_;
	// Logpoint at which partitions were last generated
	int partitionLogPoint_;
	// Grid structure holding illustrative partition data
	Grid grid_;
	// Icon of illustrative grid
	QIcon icon_;
	// Tree parent for UserCommandNode holding 'partition()' function
	Tree tree_;
	// User command nodes for varions functions
	UserCommandNode partitionFunctionNode_, partitionNameNode_, partitionOptionsNode_;
	// Whether scheme has any defined options
	bool hasOptions_;
	// Variables to hold passed coordinates
	DoubleVariable xVariable_, yVariable_, zVariable_;
	IntegerVariable idVariable_;
	// Grid size (used for last calculation of cell lists)
	Vec3<int> gridSize_;
	
	public:
	// Return whether scheme contains static partition and grid data
	bool staticData();
	// Create partition information from current grid data
	void createPartitionsFromGrid();
	// Recalculate partition information (after load or change in options)
	void recalculatePartitions();
	// Return number of partitions now recognised in grid
	int nPartitions();
	// Clear partition component lists
	void clearComponentLists();
	// Return first partition in list
	PartitionData* partitions();
	// Return nth partition in list
	PartitionData* partition(int id);
	// Return name of nth partition in list
	QString partitionName(int id);
	// Return whether the partition function has any user-definable options
	bool hasOptions();
	// Execute dialog for user-definable options in partition function
	bool showOptions();
	// Return partition in which simple (unit) coordinate falls
	int partitionId(double x, double y, double z);
	// Return the grid structure
	Grid& grid();
	// Return icon containing illustrative partitions
	QIcon& icon();
	// Set gridsize to use for calculation
	void setGridSize(Vec3<int> newSize);
	// Return last grid size used to calculated data
	Vec3<int> gridSize();
	// Copy data from specified partition
	void copy(PartitioningScheme &source);
};

ATEN_END_NAMESPACE

#endif
