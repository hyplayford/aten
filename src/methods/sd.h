/*
	*** Steepest descent minimiser
	*** src/methods/sd.h
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

#ifndef ATEN_SD_H
#define ATEN_SD_H

#include "methods/linemin.h"
#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Forward Declarations (Aten)
class Model;

// Steepest Descent Minimiser
class SDMinimiser : public LineMinimiser
{
	public:
	// Constructor
	SDMinimiser();

	private:
	// Maximum number of iterations to perform
	int nCycles_;

	public:
	// Set maximum number of cycles to perform
	void setNCycles(int i);
	// Get maximum number of  for MC move
	int nCycles() const;
	// Minimise the specified model
	double minimise(Model* sourceModel, double eConverge, double fConverge, bool simple);
};

ATEN_END_NAMESPACE

#endif
