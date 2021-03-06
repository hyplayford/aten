/*
	*** TExtraTreeWidgetItem - QTreeWidgetItem with custom data list
	*** src/gui/textratreewidgetitem.h
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

#ifndef ATEN_TEXTRATREEWIDGETITEM_H
#define ATEN_TEXTRATREEWIDGETITEM_H

#include <QtWidgets/QTreeWidget>
#include "templates/list.h"
#include "base/datastore.h"
#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Custom QTreeWidgetItem with additional data storage
class TExtraTreeWidgetItem : public QTreeWidgetItem, public DataStore
{
	public:
	// Constructors
	TExtraTreeWidgetItem(QTreeWidgetItem* parent);
	TExtraTreeWidgetItem(QTreeWidget *parent);
};

ATEN_END_NAMESPACE

#endif

