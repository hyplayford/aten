/*
        *** EPSRAto Model Plugin Functions
        *** src/plugins/io_epsr/ato_funcs.cpp
        Copyright T. Youngs 2016-2017

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

#include "plugins/io_epsr/ato.hui"
#include "plugins/io_epsr/atoexportoptions.h"
#include "model/model.h"
#include "ff/forcefield.h"
#include "base/pattern.h"
#include <base/forcefieldbound.h>
#include "templates/datapair.h"

// Constructor
EPSRAtoModelPlugin::EPSRAtoModelPlugin()
{
	// Standard Options
	standardOptions_.setSwitch(FilePluginStandardImportOptions::KeepNamesSwitch, true);

	// Plugin options
	pluginOptions_.add("temp", "300.0");
	pluginOptions_.add("vibTemp", "65.0");
	pluginOptions_.add("angTemp", "3.0");
	pluginOptions_.add("dihTemp", "10.0");
	pluginOptions_.add("eCore", "0.0");
	pluginOptions_.add("dCore", "0.0");
	pluginOptions_.add("modelGeometry", "false");
	pluginOptions_.add("individualGeometry", "false");
	pluginOptions_.add("restraintLevel", "2");
	pluginOptions_.add("writeRotations", "true");
}

// Destructor
EPSRAtoModelPlugin::~EPSRAtoModelPlugin()
{
}

/*
 * Instance Handling
 */

// Return a copy of the plugin object
BasePluginInterface* EPSRAtoModelPlugin::makeCopy() const
{
	return new EPSRAtoModelPlugin;
}

/*
 * Definition
 */

// Return type of plugin
PluginTypes::PluginType EPSRAtoModelPlugin::type() const
{
	return PluginTypes::FilePlugin;
}

// Return category of plugin
int EPSRAtoModelPlugin::category() const
{
	return PluginTypes::ModelFilePlugin;
}

// Name of plugin
QString EPSRAtoModelPlugin::name() const
{
	return QString("EPSR Ato file");
}

// Nickname of plugin
QString EPSRAtoModelPlugin::nickname() const
{
	return QString("epsrato");
}

// Return whether the plugin is enabled
bool EPSRAtoModelPlugin::enabled() const
{
	return true;
}

// Description (long name) of plugin
QString EPSRAtoModelPlugin::description() const
{
	return QString("Import/export for Empirical Potential Structure Refinement (EPSR) Ato models");
}

// Related file extensions
QStringList EPSRAtoModelPlugin::extensions() const
{
	return QStringList() << "ato";
}

// Exact names
QStringList EPSRAtoModelPlugin::exactNames() const
{
	return QStringList();
}

/*
 * Input / Output
 */

// Return whether this plugin can import data
bool EPSRAtoModelPlugin::canImport() const
{
	return true;
}

// Import data from the specified file
bool EPSRAtoModelPlugin::importData()
{
	// File header:
	// Either  1   : nmols, box length, temperature   (for cubic systems)
	//    or   2   : nmols,   temperature             (for non-cubic systems)
	// followed by : A, B, C
	//             : phib, thetac, phic
	if (!fileParser_.parseLine(Parser::SkipBlanks)) return false;
	createModel(fileParser_.filename());
	int nMols = fileParser_.argi(0);
	if (fileParser_.nArgs() == 3)
	{
		Messenger::print("File has a cubic cell");
		double boxSize = fileParser_.argd(1);
		targetModel()->setCell(Vec3<double>(boxSize,boxSize,boxSize), Vec3<double>(90,90,90));
		setOption("temp", QString::number(fileParser_.argd(2)));
	}
	else
	{
		Messenger::print("File has a full cell specification");
		Vec3<double> lengths, angles;
		if (!fileParser_.parseLine(Parser::SkipBlanks)) return false;
		lengths = fileParser_.arg3d(0);
		if (!fileParser_.parseLine(Parser::SkipBlanks)) return false;
		angles = fileParser_.arg3d(0);

		// angles.x = phib = angle between a and b (== gamma)
		// angles.y = thetac = angle deviation from cartesian z (== 90-beta)  ?? CHECK
		// angles.z = phic = angle deviation from cartesian z (== 90-alpha)  ?? CHECK
		targetModel()->setCell(lengths, Vec3<double>(90-angles.z, 90-angles.y, angles.x));
	}

	// 2 : step sizes etc. (ignored - we will read them in properly at the end of the file)
	if (!fileParser_.parseLine()) return false;

	// Molecule/atom specifications are in the form:
	// n  : natoms, comx, comy, comz, phix, phiy, phiz
	// n+1: atom name 1
	// n+2: x,y,z (offsets from com)
	// n+3: nrestraints, res1, res2... (number of distance restraints, 5 per line)
	// n+4: ...resN-1, resN
	// n+5: nrot (number of defined molecular rotations)
	// n+6: atom1, atom2 (bonds of rotation 'axis')
	// n+7: list of headgroup atoms that are rotated
	int atomOffset = 0;
	int nAtoms, nRestraints, currentArg, partnerId;
	Vec3<double> com, delta;
	QString atomName;
	for (int m=0; m<nMols; m++)
	{
		if (!fileParser_.parseLine()) return false;
		nAtoms = fileParser_.argi(0);
		com = fileParser_.arg3d(1);

		for (int n=0; n<nAtoms; n++)
		{
			// Atom name
			if (!fileParser_.parseLine()) return false;
			atomName = fileParser_.argc(0);

			// Atom coordinates (specified as offset from com)
			if (!fileParser_.parseLine()) return false;
			delta = fileParser_.arg3d(0);

			// Create a new atom with element 0 - it will be set to a proper element later on - and store the atom name in its data member
			Atom* i = createAtom(targetModel(), atomName, com+delta);

			// Read in number of restraints line
			if (!fileParser_.parseLine()) return false;
			nRestraints = fileParser_.argi(0);
			currentArg = 1;
			while (nRestraints > 0)
			{
				// Look at next available argument - if none, read another line in
				if (currentArg >= fileParser_.nArgs())
				{
					if (!fileParser_.parseLine()) return false;
					currentArg = 0;
				}
				partnerId = fileParser_.argi(currentArg) - 1;
				currentArg += 2;

				// Create new bond between these atoms (only if the partnerId is less than the current atom index)
				if (partnerId < n) targetModel()->bondAtoms(atomOffset+n, atomOffset+partnerId, Bond::Single);

				--nRestraints;
			}
		}

		// Discard molecular rotations and dihedrals
		// There are 14 atoms per line - first line contains number of atoms followed by (up to) 13 indices
		if (!fileParser_.parseLine()) return false;
		int nRotations = fileParser_.argi(0);
		while (nRotations > 0)
		{
			// Read line to find out which type of definition this is...
			if (!fileParser_.parseLine()) return false;

			// Skip axis line
			if (!fileParser_.skipLines(1)) return false;

			// If a DIHedral, we expect an integer which defines the number of constraints, and thus the number of lines to skip before the main
			if (fileParser_.argc(0) == "DIH")
			{
				if (!fileParser_.parseLine()) return false;
				if (!fileParser_.skipLines(fileParser_.argi(0))) return false;
			}

			// Finally, read in number of atoms affected by rotation and calculate next number of lines to discard
			if (!fileParser_.parseLine()) return false;
			if (!fileParser_.skipLines(fileParser_.argi(0)/14)) return false;

			--nRotations;
		}

		atomOffset += nAtoms;
	}

	// Atomtype specifications follow
	// If the ato file is correct, our names forcefield should contain the number of atomtypes to read in...
	Forcefield* ff = targetModel()->namesForcefield();
	if (!ff)
	{
		Messenger::warn("Ato file import ended prematurely - couldn't get names forcefield from targetModel().");
		return true;
	}
	
	// Read in until we find don't find an element symbol (i.e. a number).  
	for (int n=0; n<ff->nTypes()-1; ++n)
	{
		// Two lines per atomtype
		//   name, symbol, 0/isomass
		//   epsilon  sigma  mass  charge
		if (!fileParser_.parseLine()) return false;
		atomName = fileParser_.argc(0);
		int element = ElementMap::find(fileParser_.argc(1), ElementMap::AlphaZMap);
		if (!fileParser_.parseLine()) return false;
		double epsilon = fileParser_.argd(0);
		double sigma = fileParser_.argd(1);
		double charge = fileParser_.argd(3);

		// Search forcefield for a type with the current 'atomname'
		ForcefieldAtom* ffa = ff->findType(atomName);
		if (!ffa)
		{
			Messenger::warn("Found atomtype data for '"+atomName+"' but this name isn't in the names forcefield");
			continue;
		}

		// Set data
		ffa->setElement(element);
		ffa->setVdwForm(VdwFunctions::Lj);
		ffa->setParameter(VdwFunctions::LjEpsilon, epsilon);
		ffa->setParameter(VdwFunctions::LjSigma, sigma);
		ffa->setCharge(charge);
	}

	// Finally, loop over all atoms and set their elements according to their atomtypes
	for (Atom* i = targetModel()->atoms(); i != NULL; i = i->next)
	{
		if (!i->type()) continue;
		i->setElement(i->type()->element());
	}

	return true;
}

// Return whether this plugin can export data
bool EPSRAtoModelPlugin::canExport() const
{
	return true;
}

// Export data to the specified file
bool EPSRAtoModelPlugin::exportData()
{
	// Setup
	// -----
	// -- Must have some sort of valid pattern description
	if (!targetModel()->createPatterns()) targetModel()->createDefaultPattern();
	// -- Determine total number of molecules
	int nMols = 0;      
	for (Pattern* p = targetModel()->patterns(); p != NULL; p = p->next) nMols += p->nMolecules();

	// Line 1 : nmols, box dimension, temperature OR nmols, temperature depending on cell type
	if (!targetModel()->isPeriodic())
	{
		if (!fileParser_.writeLineF(" %7i %14.8e  %14.8e", nMols, 20.0, pluginOptions_.value("temp").toDouble())) return false;
	}
	else if (targetModel()->cell().type() == UnitCell::CubicCell)
	{
		if (!fileParser_.writeLineF(" %7i %14.8e  %14.8e", nMols, targetModel()->cell().lengths().x, pluginOptions_.value("temp").toDouble())) return false;
	}
	else
	{
		Vec3<double> lengths = targetModel()->cell().lengths();
		Vec3<double> angles = targetModel()->cell().angles();

		if (!fileParser_.writeLineF(" %7i %14.8e", nMols, pluginOptions_.value("temp").toDouble())) return false;
		if (!fileParser_.writeLineF(" %12.6e  %12.6e  %12.6e", lengths.x, lengths.y, lengths.z)) return false;
		double phib, thetac, phic;
		// Converts crystallographic alpha, beta, gamma values to spherical polar coordinates used in EPSR
		// The a vector is assumed to lie along the x-axis, and the b vector is in the x-y plane.

		// phib = angle between a/b vectors = gamma (angles.z)
		phib = angles.z;

		// Initialise thetac and phic to zero to begin with
		thetac = 0.0;
		phic = 0.0;

		// Calculate components of b and c vectors in x and y
		double bx = cos(angles.z/DEGRAD);
		double by = sin(angles.z/DEGRAD);
		double cx = cos(angles.y/DEGRAD);
		double cy = (cos(angles.x/DEGRAD) - bx*cx) / by;

		// If the angle between a and c is not 90 degrees (cx == 0.0), calculate thetac and phic
		if (fabs(cx) > 1.0e-4)
		{
			phic = atan2(cy,cx) * DEGRAD;

			// Determine angle out of z
			thetac = asin(sqrt(cx*cx + cy*cy)) * DEGRAD;
		}

		if (!fileParser_.writeLineF(" %12.6e  %12.6e  %12.6e", phib, thetac, phic)) return false;
	}

	// Line 2 : Tol, step sizes (intra trans, headgroup rot, mol rot, mol trans), vibrational temp
	// We will just write some sensible defaults here
	if (!fileParser_.writeLineF(" %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e", 0.0, 0.1, 0.3, 0.3, 1.0, pluginOptions_.value("vibTemp").toDouble(), pluginOptions_.value("angTemp").toDouble(), pluginOptions_.value("dihTemp").toDouble(), 0.0, 0.0)) return false;

	// Molecule Section
	int nRestraints, molIndex = 1;
	QString typeName;
	bool modelGeometry = pluginOptions_.value("modelGeometry") == "true";
	bool individualGeometry = modelGeometry ? (pluginOptions_.value("individualGeometry") == "true") : false;
	int restraintLevel = pluginOptions_.value("restraintLevel").toInt();
	if (restraintLevel < 1) restraintLevel = 1;
	for (Pattern* p = targetModel()->patterns(); p != NULL; p = p->next)
	{
		// Get first atom pointer and its index
		Atom* i = p->firstAtom();
		int atomOffset = i->id();

		// Setup the molecule information arrays/lists here
		List< DataPair<int, double> >* restraints = new List< DataPair<int, double> >[p->nAtoms()];
		RefList<Bond,int> uniqueBonds;
		QStringList rotationalGroups;

		// Loop over molecules in this pattern
		for (int mol = 0; mol < p->nMolecules(); ++mol)
		{
			// Write centre of mass
			Vec3<double> com = p->calculateCom(mol);
			if (!fileParser_.writeLineF("   %-2i %12.5e %12.5e %12.5e %12.5e %12.5e %12.5e F      %5i %5i", p->nAtoms(), com.x, com.y, com.z, 0.0, 0.0, 0.0, 0, molIndex)) return false;

			// Loop over atoms in molecule
			for (int n=0; n<p->nAtoms(); ++n)
			{
				// Grab type name or, if there isn't one, the element symbol
				ForcefieldAtom* ffi = i->type();
				typeName = ffi ? ffi->name() : ElementMap::symbol(i);

				// Write atom name, index, and mysterious second integer
				if (!fileParser_.writeLineF(" %-3s   %4i  %5i", qPrintable(typeName), n+1, 0)) return false;

				// Write atom offset from com
				if (!fileParser_.writeLineF(" %12.5e %12.5e %12.5e", i->r().x - com.x, i->r().y - com.y, i->r().z - com.z)) return false;

				// Write restraint information - need to calculate it first
				// Always work it out for the first molecule of a particular pattern, or if 'individualGeometry' is true
				if ((mol == 0) || individualGeometry)
				{
					Messenger::print(Messenger::Verbose, "Creating restraint information for atom %i, molecule %i of pattern '%s'...", n+1, mol+1, qPrintable(p->name()));
					nRestraints = 0;
					restraints[n].clear();
					// Loop over bonds to atom - we always restrain along bonds
					double rij, rjk, theta;
					for (RefListItem<Bond,int>* bij = i->bonds(); bij != NULL; bij = bij->next)
					{
						Atom* j = bij->item->partner(i);

						// Use forcefield information to write restraint info if possible (unless requested otherwise)
						if (modelGeometry) rij = targetModel()->distance(i, j);
						else rij = restraintDistance(i, j);
						restraints[n].add()->set(j->id() - atomOffset, rij);

						// Restrain over angles?
						if (restraintLevel >= 2) for (RefListItem<Bond,int>* bjk = j->bonds(); bjk != NULL; bjk = bjk->next)
						{
							Atom* k = bjk->item->partner(j);

							if (i == k) continue;

							// Angle formed is between atoms i-j-k
							// Use forcefield information to write restraint info if possible (unless requested otherwise)
							if (modelGeometry) restraints[n].add()->set(k->id() - atomOffset, -targetModel()->distance(i, k));
							else
							{
								// Get best restraint distance for the j-k bond
								rjk = restraintDistance(j, k);

								// Get best restraint angle for the i-j-k angle
								theta = restraintAngle(i, j, k);
								if (theta < 0.0) restraints[n].add()->set(k->id() - atomOffset, theta);
								else
								{
									// Calculate distance required to hold atoms at this angle (given we know what rij and rjk should be)
									restraints[n].add()->set(k->id() - atomOffset, -sqrt(rij*rij + rjk*rjk - 2.0*rij*rjk*cos(theta/DEGRAD)));
								}
							}
						}

						// Update list of unique bonds in this molecule (used in construction of rotational groups)
						if (mol == 0) uniqueBonds.addUnique(bij->item);
					}
				}

				// Write beginning of restraint information
				if (restraints[n].nItems() == 0)
				{
					if (!fileParser_.writeLineF(" %4i", 0)) return false;
				}
				else
				{
					if (!fileParser_.writeF(" %4i", restraints[n].nItems())) return false;
					int count = 0;
					for (int resId=0; resId<restraints[n].nItems(); ++resId)
					{
						if (!fileParser_.writeF(" %4i %9.3e", restraints[n][resId]->data1()+1, restraints[n][resId]->data2())) return false;
						++count;
						if ((count%5 == 0) || (resId == restraints[n].nItems()-1)) if (!fileParser_.writeLine()) return false;
					}
				}

				i = i->next;
			}

			// Construct rotational groups - utilise the list of unique bonds created earlier
			if (mol == 0)
			{
				// Need ring information for the pattern....
				p->findRings(prefs.maxRingSize(), -1);
				for (RefListItem<Bond,int>* ri = uniqueBonds.first(); ri != NULL; ri = ri->next)
				{
					Bond* bij = ri->item;

					Atom* i = bij->atomI();
					Atom* j = bij->atomJ();

					// Are these atoms involved in the same ring?
					if (p->atomsInRing(i,j)) continue;

					// Tree select using this bond as a reference, and find the least number of atoms to rotate
					targetModel()->selectNone(true);
					targetModel()->selectTree(i, true, false, bij);
					RefList<Atom,int> fromI = targetModel()->selectedAtoms(true);
					fromI.remove(i);
					targetModel()->selectNone(true);
					targetModel()->selectTree(j, true, false, bij);
					RefList<Atom,int> fromJ = targetModel()->selectedAtoms(true);
					fromJ.remove(j);
					RefList<Atom,int>& rotationAtoms = (fromI.nItems() > fromJ.nItems() ? fromJ : fromI);
					if (rotationAtoms.nItems() <= 1) continue;

					// Construct ROT entry and add to our list of rotational groups
					QString rot = QString(" ROT\n %1 %2\n %3 ").arg(i->id()+1, 4).arg(j->id()+1, 4).arg(rotationAtoms.nItems(), 4);
					int count = 1;
					for (int n=0; n<rotationAtoms.nItems(); ++n)
					{
						rot += QString("%1").arg(rotationAtoms[n]->item->id()+1, 4);
						if ((++count)%14 == 0)
						{
							count = 0;
							rot += "\n";
						}
						else rot += " ";
					}
					rotationalGroups << rot;
				}
			}

			// Write rotational groups
			if (!fileParser_.writeLine(QString(" %1").arg(rotationalGroups.count(),4))) return false;
			for (int n=0; n<rotationalGroups.count(); ++n) if (!fileParser_.writeLine(rotationalGroups.at(n))) return false;

			++molIndex;
			atomOffset += p->nAtoms();
		}

		// Cleanup
		delete[] restraints;
	}

	// Create and write the forcefield info (in kJ/mol)
	Forcefield typesFF;
	for (Pattern* p = targetModel()->patterns(); p != NULL; p = p->next)
	{
		// Get first atom pointer and its index
		Atom* i = p->firstAtom();
		for (int n=0; n<p->nAtoms(); ++n, i=i->next)
		{
			// Grab type name or, if there isn't one, the element symbol
			ForcefieldAtom* ffi = i->type();
			typeName = ffi ? ffi->name() : ElementMap::symbol(i);

			// Add a new type to our temporary forcefield (if we haven't already for this type)
			if (!typesFF.findType(typeName))
			{
				ForcefieldAtom* ffa = typesFF.addType(-1, typeName, typeName, i->element(), "", "");

				// Setup van der Waals for type as best we can
				ffa->setVdwForm(VdwFunctions::Lj);
				ffa->setCharge(i->charge());
				if (i->type())
				{
					if (i->type()->vdwForm() == VdwFunctions::Lj) for (int n=0; n<VdwFunctions::functionData[VdwFunctions::Lj].nParameters; ++n) ffa->setParameter(n, i->type()->parameter(n));
					else if (i->type()->vdwForm() == VdwFunctions::LjGeometric) for (int n=0; n<VdwFunctions::functionData[VdwFunctions::LjGeometric].nParameters; ++n) ffa->setParameter(n, i->type()->parameter(n));
					else Messenger::warn("Can't use currently-assigned atom type to get interaction parameters for atom %i since it is not of the correct form (%s).", i->id()+1, VdwFunctions::functionData[i->type()->vdwForm()].name);
				}
				Messenger::print("Potential data for atom %i : %s %8.4f %8.4f %8.4f", i->id()+1, qPrintable(ffa->name()), ffa->parameter(0), ffa->parameter(1), ffa->charge());
			}
		}
	}
	
	for (ForcefieldAtom* ffa = typesFF.types()->next; ffa != NULL; ffa = ffa->next)
	{

		if (!fileParser_.writeLineF(" %-3s %-3s %1i", qPrintable(ffa->name()), ElementMap::symbol(ffa->element()), 0)) return false;

		// Masses - for H atoms, write deuterium mass instead
		double mass = (ffa->element() == 1 ? 2.0 : ffa->elementMass());
		if (!fileParser_.writeLineF(" %10.4e  %10.4e  %10.4e  %10.4e  %10.4e", prefs.convertEnergyTo(ffa->parameter(VdwFunctions::LjEpsilon), Prefs::KiloJoules), ffa->parameter(VdwFunctions::LjSigma), mass, ffa->charge(), 0.0)) return false;
	}

	// Intramolecular lennard jones factors (ecore / dcore)
	if (!fileParser_.writeLineF(" %10.4e  %10.4e", pluginOptions_.value("eCore").toDouble(), pluginOptions_.value("dCore").toDouble())) return false;

	// Random numbers for restart purposes
	for (int n=0; n<15; ++n) if (!fileParser_.writeF(" %i", AtenMath::randomimax())) return false;
	if (!fileParser_.writeLine("")) return false;

	// List of mol files (in our case, pattern names etc.)
	int count = 1;
	for (Pattern* p = targetModel()->patterns(); p != NULL; p = p->next)
	{
		if (!fileParser_.writeLineF(" %3i %s %13.6e %13.6e %13.6e %13.6e", count, qPrintable(p->name()), 0.0, 0.0, 0.0, 0.0)) return false;
		++count;
	}

	return true;
}

// Import next partial data chunk
bool EPSRAtoModelPlugin::importNextPart()
{
	return false;
}

// Skip next partial data chunk
bool EPSRAtoModelPlugin::skipNextPart()
{
	return false;
}

/*
 * Options
 */

// Return whether the plugin has import options
bool EPSRAtoModelPlugin::hasImportOptions() const
{
	return false;
}

// Show import options dialog
bool EPSRAtoModelPlugin::showImportOptionsDialog(KVMap& targetOptions) const
{
	return false;
}

// Return whether the plugin has export options
bool EPSRAtoModelPlugin::hasExportOptions() const
{
	return true;
}

// Show export options dialog
bool EPSRAtoModelPlugin::showExportOptionsDialog(KVMap& targetOptions) const
{
	EPSRAtoExportOptionsDialog optionsDialog(targetOptions);

	return (optionsDialog.updateAndExecute() == QDialog::Accepted);
}

/*
 * ATO Functions
 */

// Return restraint distance based on supplied ForcefieldAtoms
double EPSRAtoModelPlugin::restraintDistance(Atom* i, Atom* j)
{
	// If both ForcefieldAtoms are valid, as is the parent Forcefield, try to find the equilibrium distance from there
	if (i->type() && j->type() && i->type()->parent())
	{
		// Try to find bond in forcefield, using its equilibrium value
		ForcefieldBound* ffb = i->type()->parent()->findBond(i->type(), j->type());
		if (!ffb) return targetModel()->distance(i, j);
		else if (ffb->bondForm() == BondFunctions::Harmonic) return ffb->parameter(BondFunctions::HarmonicEq);
		else if (ffb->bondForm() == BondFunctions::Morse) return ffb->parameter(BondFunctions::MorseEq);
		else if (ffb->bondForm() == BondFunctions::Constraint) return ffb->parameter(BondFunctions::ConstraintEq);
	}

	return targetModel()->distance(i, j);
}

// Return restraint angle for supplied atoms (return negative angle if original term was a distance constraint)
double EPSRAtoModelPlugin::restraintAngle(Atom* i, Atom* j, Atom* k)
{
	// If both ForcefieldAtoms are valid, as is the parent Forcefield, try to find the equilibrium distance from there
	if (i->type() && j->type() && k->type() && i->type()->parent())
	{
		// Try to find bond in forcefield, using its equilibrium value
		ForcefieldBound* ffb = i->type()->parent()->findAngle(i->type(), j->type(), k->type());
		if (!ffb) return targetModel()->angle(i, j, k);
		else if (ffb->angleForm() == AngleFunctions::Harmonic) return ffb->parameter(AngleFunctions::HarmonicEq);
		else if (ffb->angleForm() == AngleFunctions::Cosine) return ffb->parameter(AngleFunctions::CosineEq);
		else if (ffb->angleForm() == AngleFunctions::HarmonicCosine) return ffb->parameter(AngleFunctions::HarmonicCosineEq);
		else if (ffb->angleForm() == AngleFunctions::BondConstraint) return -ffb->parameter(AngleFunctions::BondConstraintEq);
	}

	return targetModel()->angle(i, j, k);
}
