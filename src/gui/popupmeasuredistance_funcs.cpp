/*
	*** Popup Widget - Measure Distance Functions
	*** src/gui/popupmeasuredistance_funcs.cpp
	Copyright T. Youngs 2007-2015

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

#include "gui/popupmeasuredistance.h"
#include "main/aten.h"
#include "gui/mainwindow.h"
#include "parser/commandnode.h"
#include "base/namespace.h"

ATEN_USING_NAMESPACE

// Constructor
MeasureDistancePopup::MeasureDistancePopup(AtenWindow& parent, TMenuButton* buttonParent) : TMenuButtonPopupWidget(buttonParent), parent_(parent)
{
	// Set up interface
	ui.setupUi(this);
}

// Show popup, updating any controls as necessary beforehand
void MeasureDistancePopup::popup()
{
	refreshing_ = true;

	show();

	refreshing_ = false;
}

// Call named method associated to popup
bool MeasureDistancePopup::callMethod(QString methodName, ReturnValue& rv)
{
	if (methodName == "TEST") return true;
	else if (methodName == "hideEvent")
	{
		return true;
	}
	else printf("No method called '%s' is available in this popup.\n", qPrintable(methodName));
	return false;
}

/*
 * Widget Functions
 */

void MeasureDistancePopup::on_SelectionButton_clicked(bool checked)
{
	// Run command
	CommandNode::run(Commands::MeasureSelected, "i", 2);

	// Update display
	parent_.updateWidgets(AtenWindow::MainViewTarget);

	// Hide popup
	done();
}
