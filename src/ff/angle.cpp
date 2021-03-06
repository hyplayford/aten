/*
	*** Angle energy / force calculation
	*** src/ff/angle.cpp
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

#include "model/model.h"
#include "base/forcefieldbound.h"
#include "base/pattern.h"

ATEN_USING_NAMESPACE

// Calculate angle energy of pattern (or individual molecule if 'molecule' != -1)
void Pattern::angleEnergy(Model* srcmodel, EnergyStore* estore, int molecule)
{
	Messenger::enter("Pattern::angleEnergy");
	static int i,j,k,aoff,m1;
	static double forcek, n, s, eq, rij, theta, energy, c0, c1, c2;
	static double coseq, delta;
	static ForcefieldBound* ffb;
	static PatternBound* pb;
	static Vec3<double> vecij, veckj;
	energy = 0.0;
	aoff = (molecule == -1 ? startAtom_ : startAtom_ + molecule*nAtoms_);
	for (m1=(molecule == -1 ? 0 : molecule); m1<(molecule == -1 ? nMolecules_ : molecule+1); m1++)
	{
		for (pb = angles_.first(); pb != NULL; pb = pb->next)
		{
			// Grab atom indices and calculate angle (in radians)
			i = pb->atomId(0) + aoff;
			j = pb->atomId(1) + aoff;
			k = pb->atomId(2) + aoff;
			theta = srcmodel->angle(i,j,k) / DEGRAD;
			// Grab pointer to function data
			ffb = pb->data();
			// Calculate energy contribution
			switch (pb->data()->angleForm())
			{
				case (AngleFunctions::None):
					Messenger::print("Warning: No function is specified for angle energy %i-%i-%i.", i, j, k);
				case (AngleFunctions::Ignore):
					break;
				case (AngleFunctions::Harmonic): 
					// U(theta) = 0.5 * forcek * (theta - eq)**2
					forcek = ffb->parameter(AngleFunctions::HarmonicK);
					eq = ffb->parameter(AngleFunctions::HarmonicEq) / DEGRAD;
					theta -= eq;
					energy += 0.5 * forcek * theta * theta;
					break;
				case (AngleFunctions::Cosine):
					// U(theta) = forcek * (1 + s * cos(n*theta - eq))
					forcek = ffb->parameter(AngleFunctions::CosineK);
					eq = ffb->parameter(AngleFunctions::CosineEq) / DEGRAD;
					n = ffb->parameter(AngleFunctions::CosineN);
					s = ffb->parameter(AngleFunctions::CosineS);
					energy += forcek * (1.0 + s * cos(n * theta - eq));
					break;
				case (AngleFunctions::Cos2):
					// U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
					forcek = ffb->parameter(AngleFunctions::Cos2K);
					c0 = ffb->parameter(AngleFunctions::Cos2C0);
					c1 = ffb->parameter(AngleFunctions::Cos2C1);
					c2 = ffb->parameter(AngleFunctions::Cos2C2);
					energy += forcek * (c0 + c1 * cos(theta) + c2 * cos(2.0 * theta));
					break;
				case (AngleFunctions::HarmonicCosine):
					// U(theta) = 0.5 * forcek * (cos(theta) - cos(eq)))**2
					forcek = ffb->parameter(AngleFunctions::HarmonicCosineK);
					coseq = cos(ffb->parameter(AngleFunctions::HarmonicCosineEq) / DEGRAD);
					delta = cos(theta) - coseq;
					energy += 0.5 * forcek * delta * delta;
					break;
				case (AngleFunctions::BondConstraint):
					// U = 0.5 * forcek * (r - eq)**2
					forcek = fabs(ffb->parameter(AngleFunctions::BondConstraintK));
					eq = ffb->parameter(AngleFunctions::BondConstraintEq);
					rij = srcmodel->distance(i, k) - eq;
					energy += 0.5 * forcek * rij * rij;
					break;
				default:
					Messenger::print("No equation coded for angle energy of type '%s'.", AngleFunctions::functionData[pb->data()->angleForm()].name);
					break;

			}
		}
		aoff += nAtoms_;
	}
	// Increment energy for pattern
	estore->add(EnergyStore::AngleEnergy,energy,id_);
	Messenger::exit("Pattern::angleEnergy");
}

// Calculate angle forces in pattern
void Pattern::angleForces(Model* srcmodel)
{
	Messenger::enter("Pattern::angleForcess");
	int i,j,k,aoff,m1;
	Vec3<double> vec_ji, vec_jk, fi, fj, fk, vec_ik;
	double forcek, eq, dp, theta, mag_ij, mag_kj, n, s, c1, c2, cosx, rij;
	double du_dtheta, dtheta_dcostheta;
	ForcefieldBound* ffb;
	PatternBound* pb;
	Atom** modelatoms = srcmodel->atomArray();
	UnitCell& cell = srcmodel->cell();
	aoff = startAtom_;
	for (m1=0; m1<nMolecules_; m1++)
	{
		for (pb = angles_.first(); pb != NULL; pb = pb->next)
		{
			// Grab atomic indices and calculate angle (in radians)
			i = pb->atomId(0) + aoff;
			j = pb->atomId(1) + aoff;
			k = pb->atomId(2) + aoff;
			// Minimum image w.r.t. atom j
			vec_ji = cell.mimVector(modelatoms[j]->r(),modelatoms[i]->r());
			vec_jk = cell.mimVector(modelatoms[j]->r(),modelatoms[k]->r());
			// Normalise vectors, calculate dot product and angle.
			mag_ij = vec_ji.magAndNormalise();
			mag_kj = vec_jk.magAndNormalise();
			dp = vec_ji.dp(vec_jk);
			theta = acos(dp);
			dtheta_dcostheta = -1.0 / sin(theta);
			ffb = pb->data();
			// Generate forces
			switch (pb->data()->angleForm())
			{
				case (AngleFunctions::None):
					Messenger::print("Warning: No function is specified for angle force %i-%i-%i.", i, j, k);
				case (AngleFunctions::Ignore):
					du_dtheta = 0.0;
					break;
				case (AngleFunctions::Harmonic): 
					// dU/d(theta) = forcek * (theta - eq)
					forcek = ffb->parameter(AngleFunctions::HarmonicK);
					eq = ffb->parameter(AngleFunctions::HarmonicEq) / DEGRAD;
					du_dtheta = forcek * (theta - eq);
					break;
				case (AngleFunctions::Cosine):
					// dU/d(theta) = -forcek * n * s * sin(n*theta - eq)
					forcek = ffb->parameter(AngleFunctions::CosineK);
					eq = ffb->parameter(AngleFunctions::CosineEq) / DEGRAD;
					n = ffb->parameter(AngleFunctions::CosineN);
					s = ffb->parameter(AngleFunctions::CosineS);
					du_dtheta = -forcek * n * s * sin(n * theta - eq);
					break;
				case (AngleFunctions::Cos2):
					// dU/d(theta) = -forcek * (c1 * sin(theta) + 2 * c2 * sin(2*theta))
					forcek = ffb->parameter(AngleFunctions::Cos2K);
					c1 = ffb->parameter(AngleFunctions::Cos2C1);
					c2 = ffb->parameter(AngleFunctions::Cos2C2);
					du_dtheta = -forcek * (c1 * sin(theta) + 2.0 * c2 * sin(2.0 * theta));
					break;
				case (AngleFunctions::HarmonicCosine):
					// dU/d(theta) = forcek * (cos(theta) - cos(eq))) * -sin(theta)
					forcek = ffb->parameter(AngleFunctions::HarmonicCosineK);
					cosx = cos(ffb->parameter(AngleFunctions::HarmonicCosineEq) / DEGRAD);
					du_dtheta = -forcek * (cos(theta) - cosx) * sin(theta);
					break;
				case (AngleFunctions::BondConstraint):
					// dU/dr = forcek * (r - eq)
					forcek = ffb->parameter(AngleFunctions::BondConstraintK);
					eq = ffb->parameter(AngleFunctions::BondConstraintEq);
					vec_ik = cell.mimVector(modelatoms[i]->r(), modelatoms[k]->r());
					rij = vec_ik.magnitude();
					du_dtheta = -forcek * (rij - eq);
					break;
				default:
					Messenger::print("No equation coded for angle force of type '%s'.", AngleFunctions::functionData[pb->data()->angleForm()].name);
					break;
			}

			// Exception for BondConstraint term...
			if (pb->data()->angleForm() == AngleFunctions::BondConstraint)
			{
				fi = -(vec_ik / rij) * du_dtheta;
				fj = 0.0;
				fk = -fi;
			}
			else
			{
				// Complete chain rule
				du_dtheta *= dtheta_dcostheta;
				// Calculate atomic forces
				fi = vec_jk - vec_ji * dp;
				fi *= -du_dtheta / mag_ij;
				fk = vec_ji - vec_jk * dp;
				fk *= -du_dtheta / mag_kj;
				fj = fi + fk;
			}

			// Add contributions into force arrays
			modelatoms[i]->f() += fi;
			modelatoms[j]->f() -= fj;
			modelatoms[k]->f() += fk;
		}
		aoff += nAtoms_;
	}
	Messenger::exit("Pattern::angleForcess");
}
