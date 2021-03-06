/*
	*** Atom
	*** src/base/atom.cpp
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

#include "base/atom.h"
#include "base/bond.h"
#include "base/sysfunc.h"
#include "base/forcefieldatom.h"
#include "model/model.h"
#include "base/elementmap.h"
#include "base/namespace.h"

ATEN_USING_NAMESPACE

// Atom labels
const char* AtomLabelKeywords[Atom::nLabelTypes] = { "id", "element", "type", "ffequiv", "charge" };
const char* AtomLabelNiceKeywords[Atom::nLabelTypes] = { "ID", "Element", "Type", "FF Equiv", "Charge" };
Atom::AtomLabel Atom::atomLabel(QString s, bool reportError)
{
	Atom::AtomLabel al = (Atom::AtomLabel) enumSearch("atom label", Atom::nLabelTypes, AtomLabelKeywords, s, reportError);
	if ((al == Atom::nLabelTypes) && reportError) enumPrintValid(Atom::nLabelTypes,AtomLabelKeywords);
	return al;
}
const char* Atom::atomLabel(Atom::AtomLabel al)
{
	return AtomLabelKeywords[al];
}
const char* Atom::atomLabelNice(Atom::AtomLabel al)
{
	return AtomLabelNiceKeywords[al];
}

// Atom environment
const char* AtomEnvironmentText[Atom::nEnvironments] = { "unspecified", "unbound", "pure", "nonpure", "aromatic" };
const char* Atom::atomEnvironment(Atom::AtomEnvironment ae)
{
	return AtomEnvironmentText[ae];
}

// Geometries about atomic centres
const char* AtomGeometryKeywords[Atom::nAtomGeometries] = { "unspecified", "unbound", "onebond", "linear", "tshape", "trigonal", "tetrahedral", "sqplanar", "tbp", "octahedral" };
int AtomGeometryNBonds[Atom::nAtomGeometries] = { 0, 0, 1, 2, 3, 3, 4, 4, 5, 6 };
Atom::AtomGeometry Atom::atomGeometry(QString s, bool reportError)
{
	Atom::AtomGeometry ag = (Atom::AtomGeometry) enumSearch("atom geometry",Atom::nAtomGeometries,AtomGeometryKeywords,s, reportError);
	if ((ag == Atom::nAtomGeometries) && reportError) enumPrintValid(Atom::nAtomGeometries,AtomGeometryKeywords);
	return ag;
}
const char* Atom::atomGeometry(Atom::AtomGeometry i)
{
	return AtomGeometryKeywords[i];
}
int Atom::atomGeometryNBonds(Atom::AtomGeometry ag)
{
	return AtomGeometryNBonds[ag];
}

// Constructor
Atom::Atom() : ListItem<Atom>()
{
	// Private variables
	charge_ = 0.0;
	element_ = 0;
	os_ = 0;
	environment_ = Atom::NoEnvironment;
	type_ = NULL;
	fixedType_ = false;
	fixedPosition_ = false;
	selected_ = false;
	hidden_ = false;
	marked_ = false;
	style_ = Prefs::LineStyle;
	labels_ = 0;
	parent_ = NULL;
	id_ = -1;
	data_ = NULL;
	tempBit_ = 0;
	// Set initial custom colour to be black (since we have no element yet)
	colour_[0] = 0.0;
	colour_[1] = 0.0;
	colour_[2] = 0.0;
	colour_[3] = 1.0;
}

// Destructor
Atom::~Atom()
{
	if (data_ != NULL) delete[] data_;
}

// Set parent model
void Atom::setParent(Model* m)
{
	parent_ = m;
}

// Return parent model
Model* Atom::parent()
{
	return parent_;
}

/*
 * Coordinates
 */

Vec3<double>& Atom::r()
{
	return r_;
}

/*
 * Forces
 */

Vec3<double>& Atom::f()
{
	return f_;
}

/*
 * Velocities
 */

Vec3<double>& Atom::v()
{
	return v_;
}

/*
 * Character
 */

// Sets the atom charge
void Atom::setCharge(double d)
{
	charge_ = d;
}

// Return the atom charge
double Atom::charge() const
{
	return charge_;
}

// Set the element type of the atom
void Atom::setElement(short int newel)
{
	element_ = newel;
}

// Return the element of the atom
short int Atom::element() const
{
	return element_;
}

// Check element against the supplied value
bool Atom::isElement(short int n) const
{
	return (n == element_);
}

// Check oxidation state against supplied value
bool Atom::isOs(short int n) const
{
	return (n == os_);
}

// Return the oxidation state of the atom
short int Atom::os() const
{
	return os_;
}

// Set the forcefield type of the atom
void Atom::setType(ForcefieldAtom* ffa)
{
	type_ = ffa;
}

// Return the forcefield type of the atom
ForcefieldAtom* Atom::type() const
{
	return type_;
}

// Set the fixed status of the assigned atom type
void Atom::setTypeFixed(bool b)
{
	fixedType_ = b;
}

// Return the fixed status of the assigned atom type
bool Atom::hasFixedType() const
{
	return fixedType_;
}

// Set whether the atom's position is fixed
void Atom::setPositionFixed(bool b)
{
	fixedPosition_ = b;
}

// Return whether the atom's position is fixed
bool Atom::isPositionFixed() const
{
	return fixedPosition_;
}

// Return number of attached hydrogens
int Atom::nHydrogens()
{
	int nh = 0;
	for (RefListItem<Bond,int>* bref = bonds_.first(); bref != NULL; bref = bref->next) if (bref->item->partner(this)->element() == 1) nh++;
	return nh;
}

// Check the ff type of the atom against the supplied value
bool Atom::typeIs(ForcefieldAtom* type) const
{
	return (type == type_);
}

// Set the environment of the atom
void Atom::setEnvironment(Atom::AtomEnvironment ae)
{
	environment_ = ae;
}

// Return the environment of the atom
Atom::AtomEnvironment Atom::environment() const
{
	return environment_;
}

// Check the environment of the atom against the supplied value
bool Atom::isEnvironment(Atom::AtomEnvironment ae) const
{
	return (environment_ == ae);
}

// Reset data in structure
void Atom::reset()
{
	element_ = 0;
	r_.zero();
	f_.zero();
	v_.zero();
	id_ = -1;
}

// Copy atom data
bool Atom::copy(Atom* source)
{
	if (source == NULL) return false;
	r_ = source->r_;
	f_ = source->f_;
	v_ = source->v_;
	charge_ = source->charge_;
	element_ = source->element_;
	style_ = source->style_;
	environment_ = source->environment_;
	type_ = source->type_;
	fixedType_ = source->fixedType_;
	labels_ = source->labels_;
	hidden_ = source->hidden_;
	fixedPosition_ = source->fixedPosition_;
	for (int n=0; n<4; ++n) colour_[n] = source->colour_[n];
	hidden_ = source->hidden_;
	// Do NOT copy selection or marked state (set to false)
	selected_ = false;
	marked_ = false;
	return true;
}

// Copy atom style
void Atom::copyStyle(Atom* source)
{
	style_ = source->style_;
	hidden_ = source->hidden_;
	colour_[0] = source->colour_[0];
	colour_[1] = source->colour_[1];
	colour_[2] = source->colour_[2];
	colour_[3] = source->colour_[3];	
}

// Print
void Atom::print() const
{
	// Note: We print the 'visual' id (id_ + 1) and not the internal id (id_)
	Messenger::print("Atom ID %i (%s):", id_+1, ElementMap::name(element_));
	Messenger::print(" %s, %s, individual style is %s.", (selected_ ? "Selected" : "Not selected"), (hidden_ ? "hidden" : "not hidden"), Prefs::drawStyle(style_));
	Messenger::print(" Coordinates : %8.4f %8.4f %8.4f",r_.x,r_.y,r_.z);
	Messenger::print("  Velocities : %8.4f %8.4f %8.4f",v_.x,v_.y,v_.z);
	Messenger::print("      Forces : %8.4f %8.4f %8.4f",f_.x,f_.y,f_.z);
	Messenger::print("      Charge : %8.4f",charge_);
	Messenger::print("      FFType : %s", (type_ != NULL ? qPrintable(type_->name()) : "None"));
	Messenger::print("       Bonds : %i", bonds_.nItems());
	Messenger::print(" Environment : %s", Atom::atomEnvironment(environment_));
	Messenger::print("        O.S. : %i",os_);
}

// Print summary
void Atom::printSummary() const
{
	// Print format :" Id     El   FFType   FFId          X             Y             Z              Q       Sel Fix \n");
	// Note: We print the 'visual' id (id_ + 1) and not the internal id (id_)
	Messenger::print(" %-5i  %-3s  %-8s %-6i %13.6e %13.6e %13.6e  %13.6e  %c  %c%c", id_+1, ElementMap::symbol(element_), type_ != NULL ? qPrintable(type_->name()) : "None", type_ != NULL ? type_->typeId() : 0, r_.x, r_.y, r_.z, charge_, selected_ ? 'x' : ' ', fixedPosition_ ? 'R' : ' ', fixedType_ ? 'T' : ' ');
}

/*
 * Bonds / Bonding
 */

// Return the number of bonds to the atom
int Atom::nBonds() const
{
	return bonds_.nItems();
}

// Return the current bond list
RefListItem<Bond,int>* Atom::bonds()
{
	return bonds_.first();
}

// Return nth bond in the list
RefListItem<Bond,int>* Atom::bond(int index)
{
	if ((index < 0) || (index >= bonds_.nItems())) Messenger::print("Bond index %i is out of range for atom.", index);
	else return bonds_[index];
	return NULL;
}

// Check the number of bonds against the supplied value
bool Atom::isNBonds(int n) const
{
	return (bonds_.nItems() == n);
}

// Accept the specified bond to the atom's local reference list
void Atom::acceptBond(Bond* b)
{
	bonds_.add(b);
}

// Detach bond
void Atom::detachBond(Bond* xbond)
{
	Messenger::enter("Atom::detachBond");
	// Remove the reference to the bond from the RefList on the atom.
	bonds_.remove(xbond);
	// Mark pointer as NULL.
	if (xbond->atomI() == this) xbond->setAtomI(NULL);
	else xbond->setAtomJ(NULL);
	Messenger::exit("Atom::detachBond");
}

// Total bond order
int Atom::totalBondOrder()
{
	// Calculate the total bond order of the atom
	// Returned result is 2*actual bond order (to account for aromatic bonds [BO = 1.5])
	Messenger::enter("Atom::totalBondOrder");
	double result = 0;
	for (RefListItem<Bond,int>* bref = bonds_.first(); bref != NULL; bref = bref->next) result += bref->item->order();
	Messenger::exit("Atom::totalBondOrder");
	return int(result * 2.0 + 0.1);
}

// Find bond to atom 'j'
Bond* Atom::findBond(Atom* j)
{
	Messenger::enter("Atom::findBond");
	Bond* result = NULL;
	for (RefListItem<Bond,int>* bref = bonds_.first(); bref != NULL; bref = bref->next)
	{
		if (bref->item->partner(this) == j)
		{
			result = bref->item;
			break;
		}
	}
	Messenger::exit("Atom::findBond");
	return result;
}

// Return bond order with specified bond partner
double Atom::bondOrder(Atom* j)
{
	Messenger::enter("Atom::bondOrder");
	// First, find the bond
	Bond* b = findBond(j);
	// Criticality check
	if (b == NULL)
	{
		printf("Atom::bondOrder : No bond exists between specified atoms.\n");
		Messenger::exit("Atom::bondOrder");
		return 0.0;
	}
	Messenger::exit("Atom::bondOrder");
	return b->order();
}

// Add bound neighbours to RefList
void Atom::addBoundToRefList(RefList<Atom,int>* rlist)
{
	// Add all atoms bound to the supplied atom to the atomRefList.
	for (RefListItem<Bond,int>* bref = bonds(); bref != NULL; bref = bref->next)
		rlist->add(bref->item->partner(this), bref->item->type());
}

/*
 * Selection / Hidden / Marked
 */

// Sets the selected flag of the atom
void Atom::setSelected(bool b, bool markonly)
{
	markonly ? marked_ = b : selected_ = b;
}

// Returns the current selection state of the atom
bool Atom::isSelected(bool markonly) const
{
	return (markonly ? marked_ : selected_);
}

// Sets the hidden flag of the atom
void Atom::setHidden(bool b)
{
	hidden_ = b;
}

// Return whether the atom is hidden
bool Atom::isHidden() const
{
	return hidden_;
}

/*
 * Identity
 */

// Sets the atom id
void Atom::setId(int newid)
{
	id_ = newid;
}

// Decreases the id of the atom by 1
void Atom::decreaseId()
{
	id_ --;
}

// Return the id of the atom
int Atom::id() const
{
	return id_;
}

// Return data set for atom
const char* Atom::data()
{
	return data_;
}

// Set data for atom
void Atom::setData(const char* data)
{
	// Delete any old data and set new
	if (data_ != NULL) delete[] data_;
	data_ = NULL;
	int len = strlen(data);
	if (len == 0) return;
	data_ = new char[len+1];
	strcpy(data_, data);
}

// Clear temporary integer (or selected bit)
void Atom::clearBit()
{
	tempBit_ = 0;
}

// Add bit to temporary integer
void Atom::addBit(int bit)
{
	if (!hasBit(bit)) tempBit_ += bit;
}

// Remove bit from temporary integer
void Atom::removeBit(int bit)
{
	if (hasBit(bit)) tempBit_ -= bit;
}

// Check presence of bit in temporary integer
bool Atom::hasBit(int bit)
{
	return (tempBit_&bit);
}

// Set value of tempBit
void Atom::setBit(int value)
{
	tempBit_ = value;
}

// Return value of tempBit
int Atom::bit()
{
	return tempBit_;
}

/*
 * Rendering
 */

// Sets the drawing style of the atom
void Atom::setStyle(Prefs::DrawStyle style)
{
	style_ = style;
}

// Returns the drawing style of the atom
Prefs::DrawStyle Atom::style() const
{
	return style_;
}

// Returns true id the atom has at least one label specified
bool Atom::hasLabels() const
{
	return (labels_ == 0 ? false : true);
}

// Set label bitvector to specified value
void Atom::setLabels(short int l)
{
	labels_ = l;
}

// Returns the label bitmask of the atom
short int Atom::labels() const
{
	return labels_;
}

// Set the bit for the specified label (if it is not set already)
void Atom::addLabel(Atom::AtomLabel label)
{
	if (!(labels_&(1 << label))) labels_ += (1 << label);
}

// Unsets the bit for the specified label (if it is not unset already)
void Atom::removeLabel(Atom::AtomLabel label)
{
	if (labels_&(1 << label)) labels_ -= (1 << label);
}

// Clear all labels from the atom
void Atom::clearLabels()
{
	labels_ = 0;
}

// Set custom colour of atom
void Atom::setColour(double r, double g, double b, double a)
{
	colour_[0] = r;
	colour_[1] = g;
	colour_[2] = b;
	colour_[3] = a;
}

// Set n'th component of custom colour
void Atom::setColour(int n, double d)
{
	if ((n < 0) || (n > 4)) Messenger::print("Tried to set component %i for atom colour which is out of range.", n+1);
	else colour_[n] = d;
}

// Set custom colour from current atom element
void Atom::setColourFromElement()
{
	ElementMap::copyColour(element_, colour_);
}

// Return custom colour
double* Atom::colour()
{
	return colour_;
}

// Copy custom colour
void Atom::copyColour(Vec4<GLfloat>& col) const
{
	 col.x = (GLfloat) colour_[0];
	 col.y = (GLfloat) colour_[1];
	 col.z = (GLfloat) colour_[2];
	 col.w = (GLfloat) colour_[3];
}
