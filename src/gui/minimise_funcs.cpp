/*
	*** Qt minimiser functions interface
	*** src/gui/minimise_funcs.cpp
	Copyright T. Youngs 2007

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

#include "base/master.h"
#include "base/elements.h"
#include "methods/mc.h"
#include "methods/sd.h"
#include "gui/gui.h"
#include "gui/mainwindow.h"

// Minimisation algorithms
enum min_method { MM_STEEPEST, MM_CONJUGATE, MM_MONTECARLO, MM_SIMPLEX, MM_NITEMS };

void AtenForm::on_MinimiseButton_clicked(bool checked)
{
	double econverge, fconverge, linetol;
	int maxcycles;
	// Get the convergence values from the window controls
	econverge = pow(10,ui.EnergyConvergeSpin->value());
	fconverge = pow(10,ui.ForceConvergeSpin->value());
	maxcycles = ui.MinimiseCyclesSpin->value();
	// Perform the minimisation
	switch (ui.MinimiserMethodCombo->currentIndex())
	{
		case (MM_STEEPEST):
			master.sd.set_ncycles(maxcycles);
			master.sd.set_tolerance(pow(10,ui.SDLineToleranceSpin->value()));
			master.sd.minimise(master.get_currentmodel(),econverge,fconverge);
			break;
		case (MM_CONJUGATE):
			master.cg.set_ncycles(maxcycles);
			master.cg.set_tolerance(pow(10,ui.CGLineToleranceSpin->value()));
			master.cg.minimise(master.get_currentmodel(),econverge,fconverge);
			break;
		case (MM_MONTECARLO):
			master.mc.minimise(master.get_currentmodel(),econverge,fconverge);
			break;
		case (MM_SIMPLEX):
			msg(DM_NONE,"Simplex minimiser not yet written!\n");
			break;
	}
	gui.mainview.postredisplay();
}
