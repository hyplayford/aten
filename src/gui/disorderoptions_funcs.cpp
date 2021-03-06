/*
	*** Disordered Wizard Options
	*** src/gui/disorderoptions_funcs.cpp
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

#include "gui/disorderoptions.h"
#include "methods/mc.h"

ATEN_USING_NAMESPACE

// Constructor
DisorderOptions::DisorderOptions(QWidget* parent)
{
	ui.setupUi(this);
	// Poke current values into widgets
	ui.AccuracySpin->setValue(mc.disorderAccuracy());
	ui.NFailuresSpin->setValue(mc.disorderMaxFailures());
	ui.MinimumScaleFactorSpin->setValue(mc.disorderMinimumScaleFactor());
	ui.MaximumScaleFactorSpin->setValue(mc.disorderMaximumScaleFactor());
	ui.ReductionFactorSpin->setValue(mc.disorderReductionFactor());
	ui.MaxCyclesSpin->setValue(mc.disorderMaxCycles());
	ui.NTweaksSpin->setValue(mc.disorderNTweaks());
	ui.DistanceTweakSpin->setValue(mc.disorderDeltaDistance());
	ui.AngleTweakSpin->setValue(mc.disorderDeltaAngle());
	ui.RecoveryMaxCyclesSpin->setValue(mc.disorderRecoveryMaxCycles());
	ui.RecoveryMaxTweaksSpin->setValue(mc.disorderRecoveryMaxTweaks());
	ui.RecoveryThresholdSpin->setValue(mc.disorderRecoveryThreshold());
}

void DisorderOptions::on_CancelButton_clicked(bool checked)
{
	reject();
}

void DisorderOptions::on_OkButton_clicked(bool checked)
{
	// Store new values
	mc.setDisorderAccuracy(ui.AccuracySpin->value());
	mc.setDisorderMaxFailures(ui.NFailuresSpin->value());
	mc.setDisorderMinimumScaleFactor(ui.MinimumScaleFactorSpin->value());
	mc.setDisorderMaximumScaleFactor(ui.MaximumScaleFactorSpin->value());
	mc.setDisorderReductionFactor(ui.ReductionFactorSpin->value());
	mc.setDisorderMaxCycles(ui.MaxCyclesSpin->value());
	mc.setDisorderNTweaks(ui.NTweaksSpin->value());
	mc.setDisorderDeltaDistance(ui.DistanceTweakSpin->value());
	mc.setDisorderDeltaAngle(ui.AngleTweakSpin->value());
	mc.setDisorderRecoveryMaxCycles(ui.RecoveryMaxCyclesSpin->value());
	mc.setDisorderRecoveryMaxTweaks(ui.RecoveryMaxTweaksSpin->value());
	mc.setDisorderRecoveryThreshold(ui.RecoveryThresholdSpin->value());
	accept();
}

