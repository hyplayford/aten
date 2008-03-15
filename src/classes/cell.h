/*
	*** Periodic cell definition
	*** src/classes/cell.h
	Copyright T. Youngs 2007,2008

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

#ifndef ATEN_CELL_H
#define ATEN_CELL_H

#include "templates/vector3.h"

// Cell types
enum cell_type { CT_NONE, CT_CUBIC, CT_ORTHORHOMBIC, CT_PARALLELEPIPED, CT_NITEMS };
const char *text_from_CT(cell_type);
cell_type CT_from_text(const char *);
const char **get_CT_strings();

// Forward declarations
class atom;

// Unit cell
class unitcell
{
	public:
	// Constructor / Destructor
	unitcell();
	~unitcell();

	private:
	// Cell type
	cell_type type;

	// Vectors of the principal cell axes (rows[0] = A, rows[1] = B, rows[2] = C)
	mat3<double> axes;
	// Transpose of the principal cell axes (rows[0] = A.x,B.x,C.z)
	mat3<double> transpose;
	// Reciprocal vectors of the principal cell axes
	mat3<double> recip;
	// Inverse of axes transpose
	mat3<double> itranspose;
	// Coordinates of origin in GL space (-half cell lengths)
	vec3<double> origin;
	// Principal axis lengths
	vec3<double> lengths;
	// Angles between principal axes
	vec3<double> angles;
	// Cell / reciprocal cell volume
	double volume, rvolume;
	// Density of cell
	double density;

	public:
	// Print cell data
	void print() const;
	// Generate random position inside cell
	vec3<double> random_pos() const;
	
	/*
	// Set / Get
	*/
	private:
	// Determine the cell type from its lengths / angles
	void determine_type();

	public:
	// Remove the cell definition (i.e. set 'type' to CT_NONE)
	void reset() { type = CT_NONE; origin.zero(); }
	// Set lengths and angles and calculates matrix
	void set(const vec3<double> &lengths, const vec3<double> &angles);
	// Set matrix and calculates lengths and angles
	void set(const mat3<double> &axes);
	// Set lengths and calculates matrix
	void set_lengths(const vec3<double> &lengths) { set(lengths,angles); }
	// Set individual length
	void set_length(int i, double d) { lengths.set(i,d); }
	// Set individual angle
	void set_angle(int i, double d) { angles.set(i,d); }
	// Set individual element of axes matrix
//	void set_axes(int i, int j, double d) { axes_t.set(i,j,d); }
//	void NEWset_axes(int i, int j, double d) { axes.set(i,j,d); }
	// Return the type of cell
	cell_type get_type() const { return type; }
	// Return the cell vector matrix
	mat3<double> get_transpose() { return transpose; }
	// Return the transpose of the cell vector matrix (giving individual axis vectors in rows[])
	mat3<double> get_axes() { return axes; }
	// Return the cell vector matrix as a 4x4 matrix
	mat4<double> get_transpose_as_mat4() { return transpose.get_as_mat4(); }
	// Return a matrix of the reciprocal cell vectors
	mat3<double> get_recip() { return recip; }
	// Return the axis lengths of the cell
	vec3<double> get_lengths() { return lengths; }
	// Return the angles the cell
	vec3<double> get_angles() { return angles; }
	// Return the origin the cell
	vec3<double> get_origin() { return origin; }
	// Return the cell vectors as a column-major matrix in a 1D array
//	void get_axes_column(double* m) { axes_t.get_column_major(m); }
	void get_transpose_column(double* m) { transpose.get_column_major(m); }
	// Return the reciprocal vectors as a column-major matrix in a 1D array
	void get_recip_column(double* m) { recip.get_column_major(m); }
	// Return a inverse transpose matrix of cell axes
	mat3<double> get_inversetranspose() { return itranspose; }
	// Return the inverse of the cell vectors as a column-major matrix in a 1D array
	void get_transposeinverse_column(double *m) { itranspose.get_column_major(m); }
	// Return the volume of the cell
	double get_volume() const { return volume; }
	// Return the volume of the reciprocal cell
	double get_rvolume() const { return rvolume; }
	// Return the density of the cell
	double get_density() const { return density; }

	/*
	// Methods
	*/
	public:
	// Calculate density of cell
	void calculate_density();
	// Calculate cell reciprocal
	void calc_reciprocal();
	// Calculate inverse of axes transpose
	void calc_inverse();

	private:
	// Calculate coordinate origin of cell
	void calc_origin();

	/*
	// Atom Positioning
	*/
	public:
	// Calculate and return the fractional coordinates of the specified real position
	vec3<double> real_to_frac(const vec3<double>&) const;
	// Calculate and return the real coordinates of the specified fractional cell coordinates
	vec3<double> frac_to_real(const vec3<double>&) const;

	/*
	// Minimum image calculation
	*/
	public:
	vec3<double> mim(const vec3<double>&, const vec3<double>&) const;
	vec3<double> mimd(const vec3<double>&, const vec3<double>&) const;
	vec3<double> mim(atom*, const vec3<double>&) const;
	vec3<double> mimd(atom*, const vec3<double>&) const;
	vec3<double> mim(atom*, atom*) const;
	vec3<double> mimd(atom*, atom*) const;
	void fold(vec3<double>&) const;
	void fold(atom*) const;

	/*
	// Geometry calculation (takes fractional coords, and returns Angstroms / degrees)
	*/
	public:
	double distance(const vec3<double>&, const vec3<double>&) const;
	double distance(atom*, atom*) const;
	double angle(const vec3<double>&, const vec3<double>&, const vec3<double>&) const;
	double angle(atom*, atom*, atom*) const;
	double torsion(const vec3<double>&, const vec3<double>&, const vec3<double>&, const vec3<double>&) const;
	double torsion(atom*, atom*, atom*, atom*) const;
};

#endif
