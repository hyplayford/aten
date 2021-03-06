/*
	*** Colour scale point
	*** src/base/colourscalepoint.h
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

#ifndef ATEN_COLOURSCALEPOINT_H
#define ATEN_COLOURSCALEPOINT_H

#include "templates/list.h"
#include "base/namespace.h"
#ifdef _MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

ATEN_BEGIN_NAMESPACE

// Forward Declarations (Aten)
class Grid;
class ColourScale;
class ColourScaleDelta;

// Colour scale point
class ColourScalePoint : public ListItem<ColourScalePoint>
{
	public:
	// Constructor
	ColourScalePoint();
	// Operator!=
	bool operator!=(const ColourScalePoint& other);
	// Friend class
	friend class ColourScaleDelta;

	private:
	// Parent colourscale
	ColourScale* parent_;
	// Value at which this point occurs
	double value_;
	// Colour of this point
	double colour_[4];

	public:
	// Set parent colourscale
	void setParent(ColourScale* cscale);
	// Return parent colourscale
	ColourScale* parent();
	// Set value for scalepoint
	void setValue(double d);
	// Return value for scalepoint
	double value() const;
	// Set colour
	void setColour(double r, double g, double b, double a = 1.0);
	// Copy colour
	void copyColour(GLfloat* target) const;
	// Return pointer to colour array
	double* colour();
	// Return colour as QColor
	QColor colourAsQColor();
};

// Colour scale delta
class ColourScaleDelta : public ListItem<ColourScaleDelta>
{
	public:
	// Constructor
	ColourScaleDelta();

	private:
	// Value at which the delta starts
	double start_;
	// Range of the data from the startValue
	double delta_;
	// Colour of this starting point
	GLfloat startColour_[4];
	// Delta betweeh the starting point and the final point
	GLfloat deltaColour_[4];

	public:
	// Check whether the delta 'contains' the supplied value
	bool containsValue(double d) const;
	// Create delta from two existing colours
	void set(ColourScalePoint* point1, ColourScalePoint* point2);
	// Get colour for value, assuming that v is within the range 0 -> value_
	void colour(double v, GLfloat* target) const;
	// Return the starting value of the range
	double start() const;
	// Return the range of the delta
	double delta() const;
};

ATEN_END_NAMESPACE

#endif
