/*
	*** Double/Exponent class
	*** src/math/doubleexp.h
	Copyright T. Youngs 2013-2017

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

#ifndef ATEN_DOUBLEEXP_H
#define ATEN_DOUBLEEXP_H

#include "base/namespace.h"
#include <QString>

ATEN_BEGIN_NAMESPACE

// Mantissa/exponent class
class DoubleExp
{
	public:
	// Constructors
	DoubleExp();
	DoubleExp(double value);
	DoubleExp(double mantissa, int exponent);


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	DoubleExp& operator=(double d);
	// Equality Operator
	bool operator==(const double other);
	// Inequality Operator
	bool operator!=(const double other);


	/*
	 * Data
	 */
	private:
	// Mantissa
	double mantissa_;
	// Exponent
	int exponent_;
	// Value
	double value_;

	
	/*
	 * Functions
	 */
	private:
	// Recalculate value from stored mantissa and exponent
	void recalculate();

	public:
	// Retrieve full, real value
	double value() const;
	// Retrieve text representation of value
	QString text(int precision = 4);
	// Set mantissa and exponent
	void set(double mantissa, int exponent);
	// Set from normal value
	void set(double value);
	// Set from supplied text
	void set(QString text);
	// Set mantissa
	void setMantissa(double mantissa);
	// Return mantissa
	double mantissa() const;
	// Set exponent
	void setExponent(int exponent);
	// Return exponent
	int exponent() const;


	/*
	 * Static Member Functions
	 */
	public:
	// Return text representation of supplied value
	QString text(double value, int precision = 4);
};

ATEN_END_NAMESPACE

#endif

