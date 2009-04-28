/*
	*** Variable Types
	*** src/parser/vtypes.h
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

#ifndef ATEN_VTYPES_H
#define ATEN_VTYPES_H

// Variable description
class VTypes
{
	public:
	// Data Types
	enum DataType { NoData, IntegerData, DoubleData, StringData, VectorData, AtenData, AtomData, BondData, CellData, ElementsData, ForcefieldData, ForcefieldAtomData, ForcefieldBoundData, GridData, ModelData, PatternData, PatternBoundData, PreferencesData, nDataTypes };
	static VTypes::DataType dataType(const char *s);
	static const char *dataType(DataType);
	static const char *aDataType(DataType);
	static bool isPointer(DataType);
	static VTypes::DataType determineType(const char *s);
};

#endif
