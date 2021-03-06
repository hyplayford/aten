/*
	*** Main Window - ModelsList Functions
	*** src/gui/mainwindow_models.cpp
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

#include "main/aten.h"
#include "gui/mainwindow.h"
#include "templates/variantpointer.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

void AtenWindow::modelsListContextMenuRequested(const QPoint& point)
{
	// Get item at point
	QListWidgetItem* item = ui.ModelsList->itemAt(point);
	if (!item) return;

	// Get model pointer from item
	Model* model = VariantPointer<Model>(item->data(Qt::UserRole));
	if (!model) return;

	// Build the context menu to display
	QMenu contextMenu;
	QAction* renameAction = contextMenu.addAction("&Rename...");
	QAction* deleteAction = contextMenu.addAction("&Delete");

	// Show it
	QPoint menuPosition = ui.ModelsList->mapToGlobal(point);
	QAction* menuResult = contextMenu.exec(menuPosition);

	// What was clicked?
	if (menuResult == renameAction)
	{
		bool ok;
		QString text = QInputDialog::getText(this, tr("Rename Model: ") + model->name(), tr("New name:"), QLineEdit::Normal, model->name(), &ok);
		if (ok && !text.isEmpty())
		{
			CommandNode::run(Commands::SetName, "c", qPrintable(text));

			updateWidgets(AtenWindow::ModelsListTarget);
		}
	}
	else if (menuResult == deleteAction)
	{
		if (closeModel(model)) updateWidgets(AtenWindow::AllTargets);
	}
}

void AtenWindow::on_ModelsListToggleButton_clicked(bool checked)
{
	ui.ModelsListWidget->setVisible(checked);
	if (checked) updateWidgets(AtenWindow::ModelsListTarget);
}

void AtenWindow::on_ModelsList_itemSelectionChanged()
{
	if (refreshing_) return;

	// Loop over rows of list, setting 'visible' flags in model list accordingly
	QListWidgetItem* item;
	Model* model, *currentModel = NULL;
	for (int row = 0; row < ui.ModelsList->count(); ++row)
	{
		item = ui.ModelsList->item(row);
		model = VariantPointer<Model>(item->data(Qt::UserRole));
		if (model)
		{
			aten_.setModelVisible(model, item->isSelected());
			if (item->isSelected()) currentModel = model;
		}
	}

	// Is anything selected? If not, select one
	if (!currentModel)
	{
		if (aten_.nModels() == 0) Messenger::print("Internal Error: No model to select.");
		else currentModel = aten_.models();
	}

	// Need to set the (a) current model
	aten_.setCurrentModel(currentModel);

	updateWidgets(AtenWindow::AllTargets);
}

// Move to next model in list
void AtenWindow::on_ModelsNextButton_clicked(bool checked)
{
	Model* m = aten_.currentModel();
	aten_.setSingleModelVisible(m->next == NULL ? aten_.models() : m->next);

	updateWidgets(AtenWindow::AllTargets);
}

// Move to previous model in list
void AtenWindow::on_ModelsPreviousButton_clicked(bool checked)
{
	Model* m = aten_.currentModel();
	aten_.setSingleModelVisible(m->prev == NULL ? aten_.model(aten_.nModels()-1) : m->prev);

	updateWidgets(AtenWindow::AllTargets);
}

// Refresh model list
void AtenWindow::updateModelsList()
{
	Messenger::enter("AtenWindow::updateModelsList");

	int row;
	QListWidgetItem* item;
	Model* model, *itemModel;

	// First, iterate over existing rows in model list and remove any that aren't in our models
	row = 0;
	while (row < ui.ModelsList->count())
	{
		// Get model pointer from item, and see if its in the model list
		item = ui.ModelsList->item(row);
		model = VariantPointer<Model>(item->data(Qt::UserRole));
		if (!aten_.isModel(model))
		{
			// Not in the list anymore, so remove it from the widget
			item = ui.ModelsList->takeItem(row);
			if (item) delete item;
		}
		else
		{
			// Update current item status
			item->setSelected(model->isVisible());
			if (!model->iconIsValid()) model->setIcon(modelPixmap(model, ui.ModelsList->iconSize()));
			item->setIcon(model->icon());
			++row;
		}
	}

	// Now, iterate over the rows again, adding missing items...
	row = 0;
	for (model = aten_.models(); model != NULL; model = model->next)
	{
		// Get model pointer from current row, and see if it matches the current model
		if (row < ui.ModelsList->count()) item = ui.ModelsList->item(row);
		else item = NULL;
		itemModel = (item ? VariantPointer<Model>(item->data(Qt::UserRole)) : NULL);

		// If the model and itemModel pointers differ, create a new item at this position
		if (model != itemModel)
		{
			item = new QListWidgetItem;
			ui.ModelsList->insertItem(row, item);
		}

		// Update the current item
		item->setData(Qt::UserRole, VariantPointer<Model>(model));
		if (model->isVisible()) item->setSelected(true);
		if (!model->iconIsValid()) model->setIcon(modelPixmap(model, ui.ModelsList->iconSize()));
		item->setIcon(model->icon());

		// Increase row and move on
		++row;
	}

	Messenger::exit("AtenWindow::updateModelsList");
}

// Close specified model, saving first if requested
bool AtenWindow::closeModel(Model* m)
{
	QString text;
	if (m->isModified())
	{
		// Create a modal message dialog
		text.sprintf("Model '%s' has been modified.", qPrintable(m->name()));
		int returnvalue = QMessageBox::warning(this, "Aten", text, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
		switch (returnvalue)
		{
			// Discard changes
			case (QMessageBox::Discard):
				break;
			// Cancel close
			case (QMessageBox::Cancel):
				return false;
			// Save model before quit
			case (QMessageBox::Save):
				if (!aten_.exportModel(m, m->filename(), m->plugin())) return false;	
				break;
		}
	}

	// Remove model and update gui
	aten_.removeModel(m);

	return true;
}

// Check the status of all models, asking to save before close if necessary
bool AtenWindow::saveBeforeClose()
{
	while (aten_.models())
	{
		if (!closeModel(aten_.models())) return false;

		// Update GUI
		updateWidgets(AtenWindow::AllTargets);
	}
	return true;
}
