/*
	*** Tree GUI for Qt
	*** src/gui/treegui.h
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

#ifndef ATEN_TREEGUIQT_H
#define ATEN_TREEGUIQT_H

#include "gui/ui_treegui.h"
#include "templates/list.h"
#include "base/namespace.h"

// Forward Declarations (Qt)
class QLabel;
class QGridLayout;
class AtenWindow;

// Default height for 'single-line' widgets
#define WIDGETHEIGHT 20

ATEN_BEGIN_NAMESPACE

// Forward Declarations (Aten)
class TreeGui;
class TreeGuiWidget;

ATEN_END_NAMESPACE

ATEN_USING_NAMESPACE

// Qt/TreeGuiWidget Map Object
class QtWidgetObject : public ListItem<QtWidgetObject>
{
	public:
	// Constructor
	QtWidgetObject();
	
	private:
	// TreeGuiWidget to which Qt widget/object is associated
	TreeGuiWidget* treeGuiWidget_;
	// Associated QWidget (if not QObject)
	QWidget* qWidget_;
	// Associated QObject (if not QWidget)
	QObject *qObject_;
	// Associated label widget, if any
	QLabel *labelWidget_;
	// Text for associated label
	QString labelText_;
	// Widget's layout, if it has one
	QGridLayout *layout_;
	// Next widget position for automatic layout
	int nextLeft_, nextTop_;
	// Flag specifying whether to add widgets horizontally or vertically when using automatic fill
	bool autoFillVertical_;

	public:
	// Set TreeGuiWidget/QWidget pair
	void set(TreeGuiWidget* widget, QWidget* wid, QString label = NULL, QGridLayout *layout = NULL);
	// Set TreeGuiWidget/QObject pair
	void set(TreeGuiWidget* widget, QObject *obj);
	// Return whether currently refreshing
	bool refreshing();
	// Return TreeGuiWidget to which Qt widget/object is associated
	TreeGuiWidget* treeGuiWidget();
	// Return associated QWidget (if not QObject)
	QWidget* qWidget();
	// Return associated QObject (if not QWidget)
	QObject *qObject();
	// Return widget's layout, if it has one
	QGridLayout *layout();
	// Return whether to add widgets horizontally or vertically when using automatic fill
	bool autoFillVertical();
	// Set whether to add widgets horizontally or vertically when using automatic fill
	void setAutoFillVertical(bool b);


	/*
	 * Methods
	 */
	public:
	// Update associated QWidget / QObject based on treeGuiWidget_ data
	void updateQt();
	// Update associated CLI widget based on QWidget / QObject data
	void updateCLI();
	// Add widget to the stored layout (provided it has one) at specified geometry
	bool addWidget(QtWidgetObject* qtwo, int l, int t, int addToWidth, int addToHeight);
	// Add widget to the stored layout (provided it has one) at specified geometry
	bool addSpacer(bool expandHorizontal, bool expandVertical, int l, int t, int addToWidth, int addToHeight);
};

// Tree Gui Qt Dialog
class AtenTreeGuiDialog : public QDialog
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	AtenTreeGuiDialog(TreeGui* parent = 0);
	~AtenTreeGuiDialog();
	// Main form declaration
	Ui::AtenTreeGuiDialog ui;


	/*
	 * Generic Widget Functions
	 */
	private slots:
	void buttonWidget_clicked(bool checked);
	void checkBoxWidget_clicked(bool checked);
	void comboWidget_currentIndexChanged(int row);
	void doubleSpinWidget_valueChanged(double d);
	void editWidget_editingFinished();
	void integerSpinWidget_valueChanged(int i);
	void radioButtonWidget_clicked(bool checked);
	void radioGroupWidget_buttonClicked(QAbstractButton*);
	void tabsWidget_currentChanged(int id);


	/*
	 * Widgets
	 */
	private:
	// Flag to prevent sending of update events
	bool updating_;
	// Main Grid layout
	QGridLayout *mainLayout_;
	// Associated TreeGui parent
	TreeGui* parentTree_;
	// List of Qt/TreeGui pairs
	List<QtWidgetObject> widgetObjects_;

	private:
	// Create new general layout for specified widget
	QGridLayout *addLayout(QWidget* widget);

	public:
	// Create new button widget
	QtWidgetObject* addButton(TreeGuiWidget* widget, QString label);
	// Create new checkbox widget
	QtWidgetObject* addCheck(TreeGuiWidget* widget, QString label);
	// Create new combo widget
	QtWidgetObject* addCombo(TreeGuiWidget* widget, QString label);
	// Create new dialog layout
	QtWidgetObject* addDialogLayout(TreeGui* treeGui);
	// Create new double spin widget
	QtWidgetObject* addDoubleSpin(TreeGuiWidget* widget, QString label, double step);
	// Create new edit widget
	QtWidgetObject* addEdit(TreeGuiWidget* widget, QString label);
	// Create new frame widget
	QtWidgetObject* addFrame(TreeGuiWidget* widget);
	// Create new group box
	QtWidgetObject* addGroup(TreeGuiWidget* widget, QString label);
	// Create new integer spin widget
	QtWidgetObject* addIntegerSpin(TreeGuiWidget* widget, QString label, int step);
	// Create new label widget
	QtWidgetObject* addLabel(TreeGuiWidget* widget, QString text);
	// Create new page (only in tab widget)
	QtWidgetObject* addPage(TreeGuiWidget* widget, TreeGuiWidget* tabWidget, QString label);
	// Create new radio button
	QtWidgetObject* addRadioButton(TreeGuiWidget* widget, TreeGuiWidget* groupWidget, QString name, QString label, int id);
	// Create new (invisible) radio group
	QtWidgetObject* addRadioGroup(TreeGuiWidget* widget);
	// Create new stack widget
	QtWidgetObject* addStack(TreeGuiWidget* widget);
	// Create new tab widget
	QtWidgetObject* addTabs(TreeGuiWidget* widget);
	// Execute (show) dialog with supplied title
	bool execute(QString title);
};

#endif
