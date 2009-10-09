/*
	*** Qt GUI: Cell Transform Window
	*** src/gui/celltransform_funcs.cpp
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

#include "main/aten.h"
#include "model/model.h"
#include "gui/mainwindow.h"
#include "gui/gui.h"
#include "gui/celltransform.h"
#include "parser/commandnode.h"

// Constructor
AtenCellTransform::AtenCellTransform(QWidget *parent, Qt::WindowFlags flags) : QDialog(parent,flags)
{
	ui.setupUi(this);

	// Private variables
	refreshing_ = FALSE;
}

// Destructor
AtenCellTransform::~AtenCellTransform()
{
}

// Show window
void AtenCellTransform::showWindow()
{
	//if (shouldRefresh_) refresh();
	show();
}

// Refresh window
void AtenCellTransform::refresh()
{
	// Set label to show cell volume (do this before early exit check so we update the cell volume after widget-enforced cell changes)
	Model *m = aten.currentModel();
	Cell::CellType ct = m->cell()->type();
	if (refreshing_) return;
	else refreshing_ = TRUE;
	// Update checkboxes in replicate group
	ui.CellReplicateFoldCheck->setChecked( prefs.replicateFold() );
	ui.CellReplicateTrimCheck->setChecked( prefs.replicateTrim() );
	// Update the widgets on the page to reflect the current model's unit cell
	if (ct == Cell::NoCell)
	{
		// No cell, so disable group boxes and quit
		ui.CellTransformTabs->setEnabled(FALSE);
		refreshing_ = FALSE;
		return;
	}
	else
	{
		// Activate widgets
		ui.CellTransformTabs->setEnabled(TRUE);
	}
	refreshing_ = FALSE;
}

/*
// Replicate Functions
*/

void AtenCellTransform::on_CellReplicateButton_clicked(bool checked)
{
	CommandNode::run(Command::Replicate, "dddddd", ui.CellReplicateNegXSpin->value(), ui.CellReplicateNegYSpin->value(), ui.CellReplicateNegZSpin->value(), ui.CellReplicatePosXSpin->value(), ui.CellReplicatePosYSpin->value(),  ui.CellReplicatePosZSpin->value());
	gui.update();
}

void AtenCellTransform::on_CellReplicateFoldCheck_clicked(bool checked)
{
	prefs.setReplicateFold(checked);
}

void AtenCellTransform::on_CellReplicateTrimCheck_clicked(bool checked)
{
	prefs.setReplicateTrim(checked);
}

/*
// Scale Functions
*/

void AtenCellTransform::on_CellScaleButton_clicked(bool checked)
{
	if (ui.CellScaleUseCogsCheck->isChecked()) CommandNode::run(Command::ScaleMolecules, "dddi", ui.CellScaleXSpin->value(), ui.CellScaleYSpin->value(), ui.CellScaleZSpin->value(), ui.CellScaleCalculateEnergyCheck->isChecked());
	else CommandNode::run(Command::Scale, "dddi", ui.CellScaleXSpin->value(), ui.CellScaleYSpin->value(), ui.CellScaleZSpin->value(), ui.CellScaleCalculateEnergyCheck->isChecked());
	gui.update(FALSE,TRUE,FALSE);
}

/*
// Rotate Functions
*/

void AtenCellTransform::on_CellRotateXClockwise_clicked(bool checked)
{
	// Construct rotation matrix
	Mat3<double> mat;
	mat.set(0,1.0,0.0,0.0);
	mat.set(1,1.0,cos(HALFPI),sin(HALFPI));
	mat.set(2,1.0,0.0,0.0);
}

void AtenCellTransform::on_CellRotateXAnticlockwise_clicked(bool checked)
{
}

void AtenCellTransform::on_CellRotateYClockwise_clicked(bool checked)
{
}

void AtenCellTransform::on_CellRotateYAnticlockwise_clicked(bool checked)
{
}

void AtenCellTransform::on_CellRotateZClockwise_clicked(bool checked)
{
}

void AtenCellTransform::on_CellRotateZAnticlockwise_clicked(bool checked)
{
}

void AtenCellTransform::dialogFinished(int result)
{
	gui.mainWindow->ui.actionCellTransformWindow->setChecked(FALSE);
}

/*
// Miller Cut Functions
*/

void AtenCellTransform::on_MillerCutButton_clicked(bool checked)
{
	CommandNode::run(Command::MillerCut, "iiii", ui.MillerHSpin->value(), ui.MillerKSpin->value(), ui.MillerLSpin->value(), ui.MillerInRadio->isChecked());
	gui.update(FALSE,TRUE,FALSE);
}

void AtenCellTransform::on_MillerSelectButton_clicked(bool checked)
{
	CommandNode::run(Command::SelectMiller, "iiii", ui.MillerHSpin->value(), ui.MillerKSpin->value(), ui.MillerLSpin->value(), ui.MillerInRadio->isChecked());
	gui.update(FALSE,TRUE,FALSE);
}

void AtenCellTransform::on_MillerHSpin_valueChanged(int value)
{
	gui.mainView.postRedisplay();
}

void AtenCellTransform::on_MillerKSpin_valueChanged(int value)
{
	gui.mainView.postRedisplay();
}

void AtenCellTransform::on_MillerLSpin_valueChanged(int value)
{
	gui.mainView.postRedisplay();
}

void AtenCellTransform::on_MillerInRadio_clicked(bool checked)
{
	gui.mainView.postRedisplay();
}

void AtenCellTransform::on_MillerOutRadio_clicked(bool checked)
{
	gui.mainView.postRedisplay();
}

