/*
	*** Eigenvector definition
	*** src/base/eigenvector.cpp
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

#include "base/eigenvector.h"
#include "math/constants.h"
#include "base/messenger.h"
#include <stdio.h>

ATEN_USING_NAMESPACE

// Constructor
Eigenvector::Eigenvector() : ListItem<Eigenvector>()
{
	// Private variables
	size_ = -1;
	eigenvector_ = NULL;
	occupancy_ = 0.0;
	isSpherical_ = false;
}

Eigenvector::~Eigenvector()
{
	if (eigenvector_ != NULL) delete[] eigenvector_;
}

// Initialise to be specified size
void Eigenvector::initialise(int size)
{
	if (eigenvector_ != NULL)
	{
		printf("Warning - Eigenvector array already exists...\n");
		delete[] eigenvector_;
	}
	size_ = size;
	eigenvector_ = new double[size_];
}

// Return size of eigenvector
int Eigenvector::size() const
{
	return size_;
}

// Return whether coefficients correspond to spherical or cartesian functions
bool Eigenvector::isSpherical()
{
	return isSpherical_;
}

// Set text name of eigenvalue
void Eigenvector::setName(QString name)
{
	name_ = name;
}

// Return text 'name' of eigenvalue
QString Eigenvector::name() const
{
	return name_;
}

// Set array index
void Eigenvector::setValue(int index, double value)
{
	if ((index < 0) || (index >= size_)) Messenger::print("Array index %i is out of range for eigenvector's array.", index);
	else eigenvector_[index] = value;
}

// Return array value specified
double Eigenvector::value(int index)
{
	if ((index < 0) || (index >= size_)) Messenger::print("Array index %i is out of range for eigenvector's array.", index);
	else return eigenvector_[index];
	return 0.0;
}

// Return array pointer
double* Eigenvector::eigenvector()
{
	if (eigenvector_ == NULL) printf("Warning - Eigenvector array has not yet been created.\n");
	return eigenvector_;
}

// Set eigenvalue
void Eigenvector::setEigenvalue(double d)
{
	eigenvalue_ = d;
}

// Return eigenvalue
double Eigenvector::eigenvalue()
{
	return eigenvalue_;
}

// Set occupancy
void Eigenvector::setOccupancy(double d)
{
	occupancy_ = d;
}

// Return occupancy
double Eigenvector::occupancy()
{
	return occupancy_;
}
