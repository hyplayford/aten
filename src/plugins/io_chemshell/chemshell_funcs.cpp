/*
        *** ChemShell Plugin Functions
        *** src/plugins/io_chemshell/chemshell_funcs.cpp
        Copyright T. Youngs 2016-2016

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

#include "plugins/io_chemshell/chemshell.hui"
#include "plugins/io_chemshell/chemshellexportoptions.h"
#include "model/model.h"

// Constructor
ChemShellModelPlugin::ChemShellModelPlugin()
{
	// Setup plugin options
	pluginOptions_.add("useTypeNames", "false");
}

// Destructor
ChemShellModelPlugin::~ChemShellModelPlugin()
{
}

/*
 * Instance Handling
 */

// Return a copy of the plugin object
BasePluginInterface* ChemShellModelPlugin::makeCopy() const
{
	return new ChemShellModelPlugin;
}

/*
 * Definition
 */

// Return type of plugin
PluginTypes::PluginType ChemShellModelPlugin::type() const
{
	return PluginTypes::FilePlugin;
}

// Return category of plugin
int ChemShellModelPlugin::category() const
{
	return PluginTypes::ModelFilePlugin;
}

// Name of plugin
QString ChemShellModelPlugin::name() const
{
	return QString("ChemShell Files");
}

// Nickname of plugin
QString ChemShellModelPlugin::nickname() const
{
	return QString("chemshell");
}

// Description (long name) of plugin
QString ChemShellModelPlugin::description() const
{
	return QString("Import/export for ChemShell input files");
}

// Related file extensions
QStringList ChemShellModelPlugin::extensions() const
{
	return QStringList() << "chemshell";
}

// Exact names
QStringList ChemShellModelPlugin::exactNames() const
{
	return QStringList();
}

/*
 * Input / Output
 */

// Return whether this plugin can import data
bool ChemShellModelPlugin::canImport() const
{
	return true;
}

// Import data from the specified file
bool ChemShellModelPlugin::importData()
{
	// Create a new Model to put our data in
	createModel();

	// Read in model data
	// TODO!

	return true;
}

// Return whether this plugin can export data
bool ChemShellModelPlugin::canExport() const
{
	return true;
}

// Export data to the specified file
bool ChemShellModelPlugin::exportData()
{
	return true;
}

// Import next partial data chunk
bool ChemShellModelPlugin::importNextPart()
{
	return false;
}

// Skip next partial data chunk
bool ChemShellModelPlugin::skipNextPart()
{
	return false;
}

/*
 * Options
 */

// Return whether the plugin has import options
bool ChemShellModelPlugin::hasImportOptions() const
{
	return false;
}

// Show import options dialog
bool ChemShellModelPlugin::showImportOptionsDialog(KVMap& targetOptions) const
{
	return false;
}

// Return whether the plugin has export options
bool ChemShellModelPlugin::hasExportOptions() const
{
	return true;
}

// Show export options dialog
bool ChemShellModelPlugin::showExportOptionsDialog(KVMap& targetOptions) const
{
	ChemShellExportOptionsDialog optionsDialog(targetOptions);

	return (optionsDialog.updateAndExecute() == QDialog::Accepted);
}
