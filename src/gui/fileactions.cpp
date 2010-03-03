/*
	*** Qt file actions
	*** src/gui/fileactions.cpp
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
#include "gui/gui.h"
#include "gui/mainwindow.h"
#include "gui/loadmodel.h"
#include "gui/selectfilter.h"
#include "gui/forcefields.h"
#include "gui/grids.h"
#include "gui/tcanvas.uih"
#include "model/model.h"
#include "base/sysfunc.h"
#include "parser/commandnode.h"

// Add new model to workspace
void AtenForm::on_actionFileNew_triggered(bool checked)
{
	Model *m = aten.addModel();
	m->enableUndoRedo();
}

// Open existing file
void AtenForm::on_actionFileOpen_triggered(bool checked)
{
	Tree *filter;
	if (gui.loadModelDialog->exec() == 1)
	{
		filter = gui.loadModelDialog->selectedFilter();
		// If filter == NULL then we didn't match a filter, i.e. the 'All files' filter was selected, and we must probe the file first.
		if (filter == NULL) filter = aten.probeFile(gui.loadModelDialog->selectedFilename(), FilterData::ModelImport);
		if (filter != NULL)
		{
			filter->executeRead(gui.loadModelDialog->selectedFilename());
			addRecent(gui.loadModelDialog->selectedFilename());
			refreshModelTabs();
			aten.currentModelOrFrame()->changeLog.add(Log::Visual);
			gui.update();
		}
	}
}

// Local save function
bool AtenForm::runSaveModelDialog()
{
	saveModelFilter = NULL;
	saveModelFilename.clear();
	Tree *filter = NULL;
	static QDir currentDirectory_(aten.workDir());
	QString filename = QFileDialog::getSaveFileName(this, "Save Model", currentDirectory_.path(),saveModelFilters);
	if (!filename.isEmpty())
	{
		// Store path for next use
		currentDirectory_.setPath(filename);
		// Grab file extension and search for it in our current lists...
		Dnchar ext = afterLastChar(qPrintable(filename), '.');
		Reflist<Tree,int> filters;
		if (ext.isEmpty())
		{
			QFileInfo fileInfo( filename );
			// Does this filename uniquely identify a specific filter?
			for (Refitem<Tree,int> *ri = aten.filters(FilterData::ModelExport); ri != NULL; ri = ri->next)
			{
				if (ri->item->filter.doesNameMatch(qPrintable(fileInfo.fileName()))) filters.add(ri->item);
			}
			msg.print(Messenger::Verbose, "Exact filename '%s' matches %i filters...", qPrintable(filename), filters.nItems());
			// If only one filter matched the filename extension, use it. Otherwise, ask for confirmation *or* list all filters.
			if (filters.nItems() != 0) filter = gui.selectFilterDialog->selectFilter("Name matches one or more model export filters.", &filters, aten.filterList(FilterData::ModelExport));
			else
			{
				filter = gui.selectFilterDialog->selectFilter("Couldn't determine format to save expression in.", NULL, aten.filterList(FilterData::ModelExport), TRUE);
				if ((filter != NULL) && gui.selectFilterDialog->appendExtension())
				{
					if (filter->filter.extensions() != NULL) filename += QString(".") + filter->filter.extensions()->get();
				}
			}
		}
		else
		{
			// Does this extension uniquely identify a specific filter?
			for (Refitem<Tree,int> *ri = aten.filters(FilterData::ModelExport); ri != NULL; ri = ri->next)
			{
				if (ri->item->filter.doesExtensionMatch(ext.get())) filters.add(ri->item);
			}
			msg.print(Messenger::Verbose, "Extension of filename '%s' matches %i filters...", qPrintable(filename), filters.nItems());
			// If only one filter matched the filename extension, use it. Otherwise, ask for confirmation *or* list all filters.
			if (filters.nItems() == 1) filter = filters.first()->item;
			else if (filters.nItems() > 1) filter = gui.selectFilterDialog->selectFilter("Extension matches one or more model export filters.", &filters, aten.filterList(FilterData::ModelExport));
			else
			{
				filter = gui.selectFilterDialog->selectFilter("Extension doesn't match any in known model export filters.", NULL, aten.filterList(FilterData::ModelExport), TRUE);
				if ((filter != NULL) && gui.selectFilterDialog->appendExtension())
				{
					if (filter->filter.extensions() != NULL) filename += QString(".") + filter->filter.extensions()->get();
				}
			}
		}
		saveModelFilter = filter;
		saveModelFilename = qPrintable(filename);
		if (filter == NULL) msg.print("No filter selected to save file '%s'. Not saved.\n", saveModelFilename.get());
		return (saveModelFilter == NULL ? FALSE : TRUE);
	}
	else return FALSE;
}

// Save current model under a different name
void AtenForm::on_actionFileSaveAs_triggered(bool checked)
{
	Model *m;
	if (runSaveModelDialog())
	{
		m = aten.currentModelOrFrame();
		m->setFilter(saveModelFilter);
		m->setFilename(saveModelFilename.get());
		if (saveModelFilter->executeWrite(saveModelFilename.get()))
		{
			m->changeLog.updateSavePoint();
			msg.print("Model '%s' saved to file '%s' (%s)\n", m->name(), saveModelFilename.get(), saveModelFilter->filter.name());
		}
		else msg.print("Failed to save model '%s'.\n", m->name());
		gui.update(FALSE,FALSE,FALSE);
	}
}

// Save current model
void AtenForm::on_actionFileSave_triggered(bool checked)
{
	// Check the filter of the current model
	// If there isn't one, or it can't export, raise the file dialog.
	// Similarly, if no filename has been set, raise the file dialog.
	Model *m = aten.currentModelOrFrame();
	Tree *t = m->filter();
	if ((t != NULL) && (t->filter.type() != FilterData::ModelExport)) t = NULL;
	Dnchar filename;
	filename = m->filename();
	if (filename.isEmpty() || (t == NULL))
	{
		if (runSaveModelDialog())
		{
			m->setFilter(saveModelFilter);
			m->setFilename(saveModelFilename.get());
			if (saveModelFilter->executeWrite(saveModelFilename.get()))
			{
				m->changeLog.updateSavePoint();
				msg.print("Model '%s' saved to file '%s' (%s)\n", m->name(), saveModelFilename.get(), saveModelFilter->filter.name());
			}
			else msg.print("Failed to save model '%s'.\n", m->name());
			//refreshModelTabs();
		}
	}
	else
	{
		t->executeWrite(filename.get());
		m->changeLog.updateSavePoint();
	}	
	gui.update(FALSE,FALSE,FALSE);
}

// Close current model
void AtenForm::on_actionFileClose_triggered(bool checked)
{
	// If the current model has been modified, ask for confirmation before we close it
	char text[512];
	Tree *filter;
	Model *m = aten.currentModel();
	if (m->changeLog.isModified())
	{
		// Create a model message dialog
		sprintf(text, "Model '%s' has been modified.\n", m->name());
		int returnvalue = QMessageBox::warning(this, "Aten", text, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
		switch (returnvalue)
		{
			// Discard changes
			case (QMessageBox::Discard):
				aten.removeModel(m);
				break;
			// Cancel quit and return to app
			case (QMessageBox::Cancel):
				return;
			// Save model before quit
			case (QMessageBox::Save):
				// If model has a filter set, just save it
				filter = m->filter();
				if (filter != NULL) filter->executeWrite(m->filename());
				else if (runSaveModelDialog())
				{
					m->setFilter(saveModelFilter);
					m->setFilename(saveModelFilename.get());
					saveModelFilter->executeWrite(saveModelFilename.get());
				}
				else return;
				aten.removeModel(m);
				break;
		}
	}
	else aten.removeModel(m);
}

// Save the current view as a bitmap image.
void AtenForm::on_actionFileSaveImage_triggered(bool checked)
{
	// Get geometry from user - initial setup is to use current canvas geometry
	char geometry[128];
	int width, height;
	bool ok;
	sprintf(geometry,"%ix%i\n", (int) gui.mainView.width(), (int) gui.mainView.height());
	QString text = QInputDialog::getText(this, tr("Image Size"), tr("Size of bitmap image (width x height) in pixels:"), QLineEdit::Normal, geometry, &ok);
	if (ok && !text.isEmpty())
	{
		strcpy(geometry,qPrintable(text));
		width = atoi(beforeChar(geometry,'x'));
		height = atoi(afterChar(geometry,'x'));
		if ((width < 1) || (height < 1))
		{
			char text[512];
			sprintf(text, "The geometry '%s' is not valid since one (or both) components are less than 1.\n", geometry);
			QMessageBox::warning(this, "Aten", text, QMessageBox::Ok);
			return;
		}
	}
	else return;
	// Get filename from user
	GuiQt::BitmapFormat bf;
	static QDir currentDirectory_(aten.workDir());
	QString filename = QFileDialog::getSaveFileName(this, "Save Bitmap", currentDirectory_.path(), saveBitmapFilters);
	if (!filename.isEmpty())
	{
		// Store path for next use
		currentDirectory_.setPath(filename);
		// Grab filename extension and search for it
		Dnchar ext = afterLastChar(qPrintable(filename), '.');
		bf = GuiQt::bitmapFormat(ext.get());
		// If we didn't recognise the extension, complain and quit
		if (bf == GuiQt::nBitmapFormats) 
		{
			char text[512];
			sprintf(text, "Bitmap format not recognised - '%s'.\n", ext.get());
			int returnvalue = QMessageBox::warning(this, "Aten", text, QMessageBox::Ok);
		}
		else if (!gui.saveImage(qPrintable(filename), bf, width, height, -1)) msg.print("Failed to save image.\n");
	}
}

// Add trajectory to model
void AtenForm::on_actionFileAddTrajectory_triggered(bool checked)
{
	Tree *filter;
	Model *m = aten.currentModel();
	static QDir currentDirectory_(aten.workDir());
	QString selFilter;
	QString filename = QFileDialog::getOpenFileName(this, "Open Trajectory", currentDirectory_.path(), loadTrajectoryFilters, &selFilter);
	if (!filename.isEmpty())
	{
		// Store path for next use
		currentDirectory_.setPath(filename);
		// Find the filter that was selected
		filter = aten.findFilterByDescription(FilterData::TrajectoryImport, qPrintable(selFilter));
		// If filter == NULL then we didn't match a filter, i.e. the 'All files' filter was selected, and we must probe the file first.
		if (filter == NULL) filter = aten.probeFile(qPrintable(filename), FilterData::TrajectoryImport);
		if (filter != NULL)
		{
			m->initialiseTrajectory(qPrintable(filename), filter);
			// Ensure trajectory toolbar is visible and View->Trajectory is selected
			ui.TrajectoryToolbar->setVisible(TRUE);
			ui.actionViewTrajectory->setChecked(TRUE);
			updateTrajectoryControls();
		}
		else msg.print( "Couldn't determine trajectory file format.\n");
		gui.update();
	}
}

// Save expression
void AtenForm::on_actionFileSaveExpression_triggered(bool checked)
{
	Tree *filter;
	static QDir currentDirectory_(aten.workDir());
	QString filename = QFileDialog::getSaveFileName(this, "Save Expression", currentDirectory_.path(), saveExpressionFilters);
	if (!filename.isEmpty())
	{
		// Store path for next use
		currentDirectory_.setPath(filename);
		// Grab file extension and search for it in our current lists...
		Dnchar ext = afterLastChar(qPrintable(filename), '.');
		// Does this extension uniquely identify a specific filter?
		Reflist<Tree,int> filters;
		if (ext.isEmpty())
		{
			QFileInfo fileInfo( filename );
			// Does this filename uniquely identify a specific filter?
			for (Refitem<Tree,int> *ri = aten.filters(FilterData::ExpressionExport); ri != NULL; ri = ri->next)
			{
				if (ri->item->filter.doesNameMatch(qPrintable(fileInfo.fileName()))) filters.add(ri->item);
			}
			msg.print(Messenger::Verbose, "Exact filename '%s' matches %i filters...", qPrintable(filename), filters.nItems());
			// If only one filter matched the filename extension, use it. Otherwise, ask for confirmation *or* list all filters.
			if (filters.nItems() != 0) filter = gui.selectFilterDialog->selectFilter("Exact name matches one or more known expression export filters.", &filters, aten.filterList(FilterData::ExpressionExport));
			else
			{
				filter = gui.selectFilterDialog->selectFilter("Couldn't determine format to save expression in.", NULL, aten.filterList(FilterData::ExpressionExport), TRUE);
				if ((filter != NULL) && gui.selectFilterDialog->appendExtension())
				{
					if (filter->filter.extensions() != NULL) filename += QString(".") + filter->filter.extensions()->get();
				}
			}
		}
		else
		{
			for (Refitem<Tree,int> *ri = aten.filters(FilterData::ExpressionExport); ri != NULL; ri = ri->next)
			{
				if (ri->item->filter.doesExtensionMatch(ext.get())) filters.add(ri->item);
			}
			msg.print(Messenger::Verbose, "Extension of filename '%s' matches %i filters...", qPrintable(filename), filters.nItems());
			// If only one filter matched the filename extension, use it. Otherwise, ask for confirmation *or* list all filters.
			if (filters.nItems() == 1) filter = filters.first()->item;
			else if (filters.nItems() > 1) filter = gui.selectFilterDialog->selectFilter("Extension matches two or more known expression export filters.", &filters, aten.filterList(FilterData::ExpressionExport));
			else
			{
				filter = gui.selectFilterDialog->selectFilter("Extension doesn't match any in known expression export filters.", NULL, aten.filterList(FilterData::ExpressionExport), TRUE);
				if ((filter != NULL) && gui.selectFilterDialog->appendExtension())
				{
					if (filter->filter.extensions() != NULL) filename += QString(".") + filter->filter.extensions()->get();
				}
			}
		}
		Model *m = aten.currentModelOrFrame();
		if (filter == NULL) msg.print("No filter selected to save file '%s'. Not saved.\n", qPrintable(filename));
		else if (filter->executeWrite(qPrintable(filename))) msg.print("Expression for model '%s' saved to file '%s' (%s)\n", m->name(), qPrintable(filename), filter->filter.name());
		else msg.print("Failed to save expression for model '%s'.\n", m->name());
	}
}

// Open grid file
void AtenForm::on_actionFileOpenGrid_triggered(bool checked)
{
	// Call routine in grids window...
	gui.gridsWindow->loadGrid();
}

// Open forcefield file
void AtenForm::on_actionFileOpenForcefield_triggered(bool checked)
{
	// Call routine in forcefields window...
	gui.forcefieldsWindow->loadForcefield();
}

// Quit program
void AtenForm::on_actionFileQuit_triggered(bool checked)
{
	if (!gui.saveBeforeClose()) return;
	saveSettings();
	gui.app->exit(0);
}

