/*
	*** Prefs Variable
	*** src/parser/prefs.cpp
	Copyright T. Youngs 2007-2017

	This file is part of Aten.

	Prefs is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Prefs is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Prefs.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser/prefs.h"
#include "parser/stepnode.h"
#include "base/prefs.h"
#include "parser/commandnode.h"
#include "main/aten.h"
#include "ff/forcefield.h"
#include "gui/mainwindow.h"
#include <render/fontinstance.h>

ATEN_USING_NAMESPACE

// Constructors
PreferencesVariable::PreferencesVariable()
{
	// Private variables
	returnType_ = VTypes::PreferencesData;
	readOnly_ = true;
}

// Destructor
PreferencesVariable::~PreferencesVariable()
{
}

/*
 * Accessors
 */

// Accessor data - name, type, arraysize, ro?
Accessor PreferencesVariable::accessorData[PreferencesVariable::nAccessors] = {
	{ "allowDialogs",		VTypes::IntegerData,		0, false },
	{ "angleLabelFormat",		VTypes::StringData,		0, false },
	{ "aromaticRingColour",		VTypes::DoubleData,		4, false },
	{ "atomStyleRadius",		VTypes::DoubleData,		Prefs::nDrawStyles, false },
	{ "backCull",			VTypes::IntegerData,		0, false },
	{ "backgroundColour",		VTypes::DoubleData,		4, false },
	{ "bondStyleRadius",		VTypes::DoubleData,		Prefs::nDrawStyles, false },
	{ "bondTolerance",		VTypes::DoubleData,		0, false },
	{ "calculateIntra",		VTypes::IntegerData,		0, false },
	{ "calculateVdw",		VTypes::IntegerData,		0, false },
	{ "chargelabelFormat",		VTypes::StringData,		0, false },
	{ "clipFar",			VTypes::DoubleData,		0, false },
	{ "clipNear",			VTypes::DoubleData,		0, false },
	{ "colourScales",		VTypes::ColourScaleData,	10, true },
	{ "correctTransparentGrids",	VTypes::IntegerData,		0, false },
	{ "dashedAromatics",		VTypes::IntegerData,		0, false },
	{ "defaultDrawStyle",		VTypes::StringData,		0, false },
	{ "densityUnit",		VTypes::StringData,		0, false },
	{ "depthCue",			VTypes::IntegerData,		0, false },
	{ "depthFar",			VTypes::IntegerData,		0, false },
	{ "depthNear",			VTypes::IntegerData,		0, false },
	{ "distanceLabelFormat",	VTypes::StringData,		0, false },
	{ "dynamicPanels",		VTypes::IntegerData,		0, false },
	{ "elecCutoff",			VTypes::DoubleData,		0, false },
	{ "elecMethod",			VTypes::StringData,		0, false },
	{ "energyUnit",			VTypes::StringData,		0, false },
	{ "ewaldAlpha",			VTypes::DoubleData,		0, false },
	{ "ewaldKMax",			VTypes::IntegerData,		3, false },
	{ "ewaldPrecision",		VTypes::DoubleData,		0, false },
	{ "fontFileName",		VTypes::StringData,		0, false },
	{ "foregroundColour",		VTypes::DoubleData,		4, false },
	{ "globeSize",			VTypes::IntegerData,		0, false },
	{ "glyphColour",		VTypes::DoubleData,		4, false },
	{ "hBonds",			VTypes::IntegerData,		0, false },
	{ "hBondDotRadius",		VTypes::DoubleData,		0, false },
	{ "hDistance",			VTypes::DoubleData,		0, false },
	{ "imageQuality",		VTypes::IntegerData,		0, false },
	{ "keyAction",			VTypes::StringData,		Prefs::nModifierKeys, false },
	{ "labelSize",			VTypes::DoubleData,		0, false },
	{ "labelDepthScaling",		VTypes::IntegerData,		0, false },
	{ "lineAliasing",		VTypes::IntegerData,		0, false },
	{ "maxCuboids",			VTypes::IntegerData,		0, false },
	{ "maxRings",			VTypes::IntegerData,		0, false },
	{ "maxRingsize",		VTypes::IntegerData,		0, false },
	{ "maxUndo",			VTypes::IntegerData,		0, false },
	{ "messagesFontSize",		VTypes::IntegerData,		0, false },
	{ "mopacExe",			VTypes::StringData,		0, false },
	{ "mouseAction",		VTypes::StringData,		Prefs::nMouseButtons, false },
	{ "mouseMoveFilter",		VTypes::IntegerData,		0, false },
	{ "multiSampling",		VTypes::IntegerData,		0, false },
	{ "noQtSettings",		VTypes::IntegerData,		0, false },
	{ "partitionGrid",		VTypes::IntegerData,		3, false },
	{ "perspective"	,		VTypes::IntegerData,		0, false },
	{ "perspectiveFOV",		VTypes::DoubleData,		0, false },
	{ "polygonAliasing",		VTypes::IntegerData,		0, false },
	{ "quality"	,		VTypes::IntegerData,		0, false },
	{ "reuseQuality",		VTypes::IntegerData,		0, false },
	{ "selectionScale",		VTypes::DoubleData,		0, false },
	{ "shininess",			VTypes::IntegerData,		0, false },
	{ "specularColour",		VTypes::DoubleData,		4, false },
	{ "spotlight",			VTypes::IntegerData,		0, false },
	{ "spotlightAmbient",		VTypes::DoubleData,		4, false },
	{ "spotlightDiffuse",		VTypes::DoubleData,		4, false },
	{ "spotlightPosition",		VTypes::DoubleData,		4, false },
	{ "spotlightSpecular",		VTypes::DoubleData,		4, false },
	{ "stickNormalWidth",		VTypes::DoubleData,		4, false },
	{ "stickSelectedWidth",		VTypes::DoubleData,		4, false },
	{ "tempDir",			VTypes::StringData,		0, false },
	{ "useWidgetForegroundBackground", VTypes::IntegerData,		0, false },
	{ "vdwCutoff",			VTypes::DoubleData,		0, false },
	{ "vibrationArrowColour",	VTypes::DoubleData,		4, false },
	{ "viewerFontFilename",		VTypes::StringData,		0, false },
	{ "viewLock",			VTypes::StringData,		0, false },
	{ "viewRotationGlobe",		VTypes::IntegerData,		0, false },
	{ "zoomThrottle",		VTypes::DoubleData,		0, false }
};

// Function data
FunctionAccessor PreferencesVariable::functionData[PreferencesVariable::nFunctions] = {
	{ ".dummy",	VTypes::IntegerData,	"",	"" }
};

// Search variable access list for provided accessor (call private static function)
StepNode* PreferencesVariable::findAccessor(QString name, TreeNode* arrayIndex, TreeNode* argList)
{
	return PreferencesVariable::accessorSearch(name, arrayIndex, argList);
}

// Private static function to search accessors
StepNode* PreferencesVariable::accessorSearch(QString name, TreeNode* arrayIndex, TreeNode* argList)
{
	Messenger::enter("PreferencesVariable::accessorSearch");
	StepNode* result = NULL;
	int i = 0;
	i = Variable::searchAccessor(name, nAccessors, accessorData);
	if (i == -1)
	{
		// No accessor found - is it a function definition?
		// for (i = 0; i < nFunctions; i++) if (strcmp(qPrintable(functionData[i].name),s) == 0) break;
		i = Variable::searchAccessor(name, nFunctions, functionData);
		if (i == -1)
		{
			Messenger::print("Error: Type 'Prefs&' has no member or function named '%s'.", qPrintable(name));
			printValidAccessors(nAccessors, accessorData, nFunctions, functionData);
			Messenger::exit("PreferencesVariable::accessorSearch");
			return NULL;
		}
		Messenger::print(Messenger::Parse, "FunctionAccessor match = %i (%s)", i, qPrintable(functionData[i].name));
		if (arrayIndex != NULL)
		{
			Messenger::print("Error: Array index given to 'Prefs&' function named '%s'.", qPrintable(name));
			Messenger::exit("PreferencesVariable::accessorSearch");
			return NULL;
		}
		// Add and check supplied arguments...
		result = new StepNode(i, VTypes::PreferencesData, functionData[i].returnType);
		result->addJoinedArguments(argList);
		if (!result->checkArguments(functionData[i].arguments, qPrintable(functionData[i].name)))
		{
			Messenger::print("Error: Syntax for 'Prefs&' function '%s' is '%s(%s)'.", qPrintable(functionData[i].name), qPrintable(functionData[i].name), qPrintable(functionData[i].argText) );
			delete result;
			result = NULL;
		}
	}
	else
	{
		Messenger::print(Messenger::Parse, "Accessor match = %i (%s)", i, qPrintable(accessorData[i].name));
		// Were we given an array index when we didn't want one?
		if ((accessorData[i].arraySize == 0) && (arrayIndex != NULL))
		{
			Messenger::print("Error: Irrelevant array index provided for member '%s'.", qPrintable(accessorData[i].name));
			result = NULL;
		}
		// Were we given an argument list when we didn't want one?
		if (argList != NULL)
		{
			Messenger::print("Error: Argument list given to 'Prefs&' array member '%s'.", qPrintable(name));
			Messenger::exit("PreferencesVariable::accessorSearch");
			return NULL;
		}
		result = new StepNode(i, VTypes::PreferencesData, arrayIndex, accessorData[i].returnType, accessorData[i].isReadOnly, accessorData[i].arraySize);
	}
	Messenger::exit("PreferencesVariable::accessorSearch");
	return result;
}

// Retrieve desired value
bool PreferencesVariable::retrieveAccessor(int i, ReturnValue& rv, bool hasArrayIndex, int arrayIndex)
{
	Messenger::enter("PreferencesVariable::retrieveAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Prefs type.\n", i);
		Messenger::exit("PreferencesVariable::retrieveAccessor");
		return false;
	}
	Accessors acc = (Accessors) i;
	// Check for correct lack/presence of array index given
	if ((accessorData[i].arraySize == 0) && hasArrayIndex)
	{
		Messenger::print("Error: Unnecessary array index provided for member '%s'.", qPrintable(accessorData[i].name));
		Messenger::exit("PreferencesVariable::retrieveAccessor");
		return false;
	}
	else if ((accessorData[i].arraySize > 0) && (hasArrayIndex))
	{
		if ((arrayIndex < 1) || (arrayIndex > accessorData[i].arraySize))
		{
			Messenger::print("Error: Array index out of bounds for member '%s' (%i, range is 1-%i).", qPrintable(accessorData[i].name), arrayIndex, accessorData[i].arraySize);
			Messenger::exit("PreferencesVariable::retrieveAccessor");
			return false;
		}
	}
	// Variables used in retrieval
	bool result;
	Prefs* ptr = (Prefs*) rv.asPointer(VTypes::PreferencesData, result);
	if ((!result) || (ptr == NULL))
	{
		Messenger::print("Invalid (NULL) %s reference encountered.", VTypes::dataType(VTypes::PreferencesData));
		result = false;
	}
	if (result) switch (acc)
	{
		case (PreferencesVariable::AllowDialogs):
			rv.set(prefs.allowDialogs());
			break;
		case (PreferencesVariable::AngleLabelFormat):
			rv.set(prefs.angleLabelFormat());
			break;
		case (PreferencesVariable::AromaticRingColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::AromaticRingColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::AromaticRingColour), 4);
			break;
		case (PreferencesVariable::AtomStyleRadius):
			if (hasArrayIndex) rv.set(ptr->atomStyleRadius( (Prefs::DrawStyle) (arrayIndex-1)) );
			else rv.setArray( VTypes::DoubleData, ptr->atomStyleRadii(), Prefs::nDrawStyles);
			break;
		case (PreferencesVariable::BackCull):
			rv.set( ptr->backfaceCulling() );
			break;
		case (PreferencesVariable::BackgroundColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::BackgroundColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::BackgroundColour), 4);
			break;
		case (PreferencesVariable::BondStyleRadius):
			if (hasArrayIndex) rv.set(ptr->bondStyleRadius( (Prefs::DrawStyle) (arrayIndex-1)) );
			else rv.setArray( VTypes::DoubleData, ptr->bondStyleRadii(), Prefs::nDrawStyles);
			break;
		case (PreferencesVariable::BondTolerance):
			rv.set(ptr->bondTolerance());
			break;
		case (PreferencesVariable::CalculateIntra):
			rv.set(ptr->calculateIntra());
			break;
		case (PreferencesVariable::CalculateVdw):
			rv.set(ptr->calculateVdw());
			break;
		case (PreferencesVariable::ChargeLabelFormat):
			rv.set(prefs.chargeLabelFormat());
			break;
		case (PreferencesVariable::ClipFar):
			rv.set(ptr->clipFar());
			break;
		case (PreferencesVariable::ClipNear):
			rv.set(ptr->clipNear());
			break;
		case (PreferencesVariable::ColourScales):
			rv.set(VTypes::ColourScaleData, &ptr->colourScale[arrayIndex-1]);
			break;
		case (PreferencesVariable::CorrectTransparentGrids):
			rv.set(ptr->correctTransparentGrids());
			break;
		case (PreferencesVariable::DashedAromatics):
			rv.set(ptr->renderDashedAromatics());
			break;
		case (PreferencesVariable::DefaultDrawStyle):
			rv.set( Prefs::drawStyle(ptr->defaultDrawStyle()) );
			break;
		case (PreferencesVariable::DensityUnit):
			rv.set(Prefs::densityUnit(ptr->densityUnit()));
			break;
		case (PreferencesVariable::DepthCue):
			rv.set( ptr->depthCue() );
			break;
		case (PreferencesVariable::DepthFar):
			rv.set( (int) ptr->depthFar());
			break;
		case (PreferencesVariable::DepthNear):
			rv.set( (int) ptr->depthNear());
			break;
		case (PreferencesVariable::DistanceLabelFormat):
			rv.set(prefs.distanceLabelFormat());
			break;
		case (PreferencesVariable::DynamicPanels):
			rv.set( ptr->dynamicPanels() );
			break;
		case (PreferencesVariable::ElecCutoff):
			rv.set( ptr->elecCutoff() );
			break;
		case (PreferencesVariable::ElecMethod):
			rv.set(Electrostatics::elecMethod(ptr->electrostaticsMethod()));
			break;
		case (PreferencesVariable::EnergyUnit):
			rv.set(Prefs::energyUnit(ptr->energyUnit()));
			break;
		case (PreferencesVariable::EwaldAlpha):
			rv.set( ptr->ewaldAlpha() );
			break;
		case (PreferencesVariable::EwaldKMax):
			if (hasArrayIndex) rv.set( ptr->ewaldKMax()[arrayIndex-1] );
			else rv.setArray(ptr->ewaldKMax());
			break;
		case (PreferencesVariable::EwaldPrecision):
			rv.set( ptr->ewaldPrecision().value() );
			break;
		case (PreferencesVariable::FontFileName):
			rv.set( ptr->viewerFontFileName() );
			break;
		case (PreferencesVariable::ForegroundColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::ForegroundColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::ForegroundColour), 4);
			break;
		case (PreferencesVariable::GlobeSize):
			rv.set(ptr->globeSize() );
			break;
		case (PreferencesVariable::GlyphDefaultColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::GlyphDefaultColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::GlyphDefaultColour), 4);
			break;
		case (PreferencesVariable::HBonds):
			rv.set( ptr->drawHydrogenBonds() );
			break;
		case (PreferencesVariable::HBondDotRadius):
			rv.set( ptr->hydrogenBondDotRadius() );
			break;
		case (PreferencesVariable::HDistance):
			rv.set( ptr->hydrogenDistance() );
			break;
		case (PreferencesVariable::ImageQuality):
			rv.set( (int) ptr->imagePrimitiveQuality() );
			break;
		case (PreferencesVariable::KeyAction):
			if (hasArrayIndex) rv.set(Prefs::keyAction( ptr->keyAction((Prefs::ModifierKey) (arrayIndex-1))) );
			else rv.setArray( VTypes::StringData, ptr->keyActionTexts(), Prefs::nModifierKeys);
			break;
		case (PreferencesVariable::LabelSize):
			rv.set( ptr->labelSize() );
			break;
		case (PreferencesVariable::LineAliasing):
			rv.set( ptr->lineAliasing() );
			break;
		case (PreferencesVariable::MaxCuboids):
			rv.set( ptr->maxCuboids() );
			break;
		case (PreferencesVariable::MaxRings):
			rv.set( ptr->maxRings() );
			break;
		case (PreferencesVariable::MaxRingSize):
			rv.set( ptr->maxRingSize() );
			break;
		case (PreferencesVariable::MaxUndo):
			rv.set( ptr->maxUndoLevels() );
			break;
		case (PreferencesVariable::MessagesFontSize):
			rv.set( ptr->messagesFont().pixelSize() );
			break;
		case (PreferencesVariable::MopacExe):
			rv.set( ptr->mopacExe() );
			break;
		case (PreferencesVariable::MouseAction):
			if (hasArrayIndex) rv.set(Prefs::mouseAction( ptr->mouseAction((Prefs::MouseButton) (arrayIndex-1))) );
			else rv.setArray( VTypes::StringData, ptr->mouseActionTexts(), Prefs::nMouseButtons);
			break;
		case (PreferencesVariable::MouseMoveFilter):
			rv.set( ptr->mouseMoveFilter() );
			break;
		case (PreferencesVariable::MultiSampling):
			rv.set( ptr->multiSampling() );
			break;
		case (PreferencesVariable::NoQtSettings):
			rv.set( ptr->loadQtSettings() );
			break;
		case (PreferencesVariable::PartitionGrid):
			if (hasArrayIndex) rv.set( ptr->partitionGridSize()[arrayIndex-1] );
			else rv.setArray(ptr->partitionGridSize());
			break;
		case (PreferencesVariable::Perspective):
			rv.set( ptr->hasPerspective() );
			break;
		case (PreferencesVariable::PerspectiveFov):
			rv.set( ptr->perspectiveFov() );
			break;
		case (PreferencesVariable::PolygonAliasing):
			rv.set( ptr->polygonAliasing() );
			break;
		case (PreferencesVariable::Quality):
			rv.set( (int) ptr->primitiveQuality() );
			break;
		case (PreferencesVariable::ReuseQuality):
			rv.set( ptr->reusePrimitiveQuality() );
			break;
		case (PreferencesVariable::SelectionScale):
			rv.set( ptr->selectionScale() );
			break;
		case (PreferencesVariable::Shininess):
			rv.set( (int) ptr->shininess() );
			break;
		case (PreferencesVariable::SpecularColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::SpecularColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::SpecularColour), 4);
			break;
		case (PreferencesVariable::Spotlight):
			rv.set( ptr->spotlightActive() );
			break;
		case (PreferencesVariable::SpotlightAmbient):
			if (hasArrayIndex) rv.set( ptr->spotlightColour(Prefs::AmbientComponent)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->spotlightColour(Prefs::AmbientComponent), 4);
			break;
		case (PreferencesVariable::SpotlightDiffuse):
			if (hasArrayIndex) rv.set( ptr->spotlightColour(Prefs::DiffuseComponent)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->spotlightColour(Prefs::DiffuseComponent), 4);
			break;
		case (PreferencesVariable::SpotlightPosition):
			if (hasArrayIndex) rv.set( ptr->spotlightPosition()[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->spotlightPosition(), 4);
			break;
		case (PreferencesVariable::SpotlightSpecular):
			if (hasArrayIndex) rv.set( ptr->spotlightColour(Prefs::SpecularComponent)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->spotlightColour(Prefs::SpecularComponent), 4);
			break;
		case (PreferencesVariable::StickNormalWidth):
			rv.set( ptr->stickLineNormalWidth() );
			break;
		case (PreferencesVariable::StickSelectedWidth):
			rv.set( ptr->stickLineSelectedWidth() );
			break;
		case (PreferencesVariable::TempDir):
			rv.set( ptr->tempDir().path() );
			break;
		case (PreferencesVariable::UseWidgetForegroundBackground):
			rv.set( ptr->useWidgetForegroundBackground() );
			break;
		case (PreferencesVariable::VdwCutoff):
			rv.set( ptr->vdwCutoff() );
			break;
		case (PreferencesVariable::VibrationArrowColour):
			if (hasArrayIndex) rv.set( ptr->colour(Prefs::VibrationArrowColour)[arrayIndex-1] );
			else rv.setArray( VTypes::DoubleData, ptr->colour(Prefs::VibrationArrowColour), 4);
			break;
		case (PreferencesVariable::ViewerFontFileName):
			rv.set( ptr->viewerFontFileName() );
			break;
		case (PreferencesVariable::ViewLock):
			rv.set( Prefs::viewLock(ptr->viewLock()) );
			break;
		case (PreferencesVariable::ViewRotationGlobe):
			rv.set( ptr->viewRotationGlobe() );
			break;
		case (PreferencesVariable::ZoomThrottle):
			rv.set( ptr->zoomThrottle() );
			break;
		default:
			printf("Internal Error: Access to member '%s' has not been defined in PreferencesVariable.\n", qPrintable(accessorData[i].name));
			result = false;
			break;
	}
	Messenger::exit("PreferencesVariable::retrieveAccessor");
	return result;
}

// Set desired value
bool PreferencesVariable::setAccessor(int i, ReturnValue& sourcerv, ReturnValue& newValue, bool hasArrayIndex, int arrayIndex)
{
	Messenger::enter("PreferencesVariable::setAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Prefs type.\n", i);
		Messenger::exit("PreferencesVariable::setAccessor");
		return false;
	}
	Accessors acc = (Accessors) i;

	// Check for correct lack/presence of array index given to original accessor, and nature of new value
	bool result = checkAccessorArrays(accessorData[acc], newValue, hasArrayIndex, arrayIndex);
	if (!result)
	{
		Messenger::exit("PreferencesVariable::setAccessor");
		return false;
	}

	// Get current data from ReturnValue
	Prefs* ptr = (Prefs*) sourcerv.asPointer(VTypes::PreferencesData, result);
	if ((!result) || (ptr == NULL))
	{
		Messenger::print("Invalid (NULL) %s reference encountered.", VTypes::dataType(VTypes::PreferencesData));
		result = false;
	}
	int n;
	bool updatePrimitives = false;
	Prefs::ColouringScheme cs;
	Prefs::DensityUnit du;
	Prefs::EnergyUnit eu;
	Electrostatics::ElecMethod em;
	Prefs::KeyAction ka;
	Prefs::MouseAction ma;
	Prefs::DrawStyle ds;
	Prefs::ViewLock vl;
	if (result) switch (acc)
	{
		case (PreferencesVariable::AllowDialogs):
			ptr->setAllowDialogs(newValue.asBool());
			break;
		case (PreferencesVariable::AngleLabelFormat):
			ptr->setAngleLabelFormat(newValue.asString());
			break;
		case (PreferencesVariable::AromaticRingColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::AromaticRingColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::AromaticRingColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::AromaticRingColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::AromaticRingColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::AtomStyleRadius):
			if (newValue.arraySize() == (Prefs::nDrawStyles - 1)) for (n=0; n<Prefs::nDrawStyles-1; ++n) ptr->setAtomStyleRadius( (Prefs::DrawStyle) n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setAtomStyleRadius( (Prefs::DrawStyle) (arrayIndex-1), newValue.asDouble(result));
			else for (n=0; n<Prefs::nDrawStyles-1; ++n) ptr->setAtomStyleRadius( (Prefs::DrawStyle) n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::BackCull):
			ptr->setBackfaceCulling(newValue.asBool());
			break;
		case (PreferencesVariable::BackgroundColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::BackgroundColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::BackgroundColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::BackgroundColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::BackgroundColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::BondStyleRadius):
			if (newValue.arraySize() == (Prefs::nDrawStyles - 1)) for (n=0; n<Prefs::nDrawStyles-1; ++n) ptr->setBondStyleRadius( (Prefs::DrawStyle) n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setBondStyleRadius( (Prefs::DrawStyle) (arrayIndex-1), newValue.asDouble(result));
			else for (n=0; n<Prefs::nDrawStyles-1; ++n) ptr->setBondStyleRadius( (Prefs::DrawStyle) n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::BondTolerance):
			ptr->setBondTolerance( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::CalculateIntra):
			ptr->setCalculateIntra( newValue.asBool() );
			break;
		case (PreferencesVariable::CalculateVdw):
			ptr->setCalculateVdw( newValue.asBool() );
			break;
		case (PreferencesVariable::ChargeLabelFormat):
			prefs.setChargeLabelFormat( newValue.asString(result) );
			break;
		case (PreferencesVariable::ClipFar):
			ptr->setClipFar( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::ClipNear):
			ptr->setClipNear( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::CorrectTransparentGrids):
			ptr->setCorrectTransparentGrids(newValue.asBool());
			break;
		case (PreferencesVariable::DashedAromatics):
			ptr->setRenderDashedAromatics(newValue.asBool());
			break;
		case (PreferencesVariable::DefaultDrawStyle):
			ds = Prefs::drawStyle( newValue.asString(result), true );
			if ((ds != Prefs::nDrawStyles) && result) ptr->setDefaultDrawStyle(ds);
			else result = false;
			break;
		case (PreferencesVariable::DensityUnit):
			du = Prefs::densityUnit( newValue.asString(result), true );
			if (du != Prefs::nDensityUnits) ptr->setDensityUnit(du);
			else result = false;
			break;
		case (PreferencesVariable::DepthCue):
			ptr->setDepthCue( newValue.asBool() );
			break;
		case (PreferencesVariable::DepthFar):
			ptr->setDepthFar( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::DepthNear):
			ptr->setDepthNear( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::DistanceLabelFormat):
			ptr->setDistanceLabelFormat(newValue.asString());
			break;
		case (PreferencesVariable::DynamicPanels):
			ptr->setDynamicPanels( newValue.asBool() );
			break;
		case (PreferencesVariable::ElecCutoff):
			ptr->setElecCutoff( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::ElecMethod):
			em = Electrostatics::elecMethod( newValue.asString(result), true );
			if (em != Electrostatics::nElectrostatics) ptr->setElectrostaticsMethod(em);
			else result = false;
			break;
		case (PreferencesVariable::EnergyUnit):
			eu = Prefs::energyUnit( newValue.asString(result), true );
			if (eu != Prefs::nEnergyUnits)
			{
				ptr->setEnergyUnit(eu);
				// Loop over stored forcefields and convert energetic parameters
				for (Forcefield* ff = aten_->forcefields(); ff != NULL; ff = ff->next) ff->convertParameters();
			}
			else result = false;
			break;
		case (PreferencesVariable::EwaldAlpha):
			ptr->setEwaldAlpha( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::EwaldKMax):
			if (newValue.arraySize() == 3) for (n=0; n<3; ++n) ptr->setEwaldKMax(n, newValue.asInteger(n, result));
			else if (hasArrayIndex) ptr->setEwaldKMax(arrayIndex-1, newValue.asInteger(result));
			else for (n=0; n<3; ++n) ptr->setEwaldKMax(n, newValue.asInteger(result));
			break;
		case (PreferencesVariable::EwaldPrecision):
			ptr->ewaldPrecision().set( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::FontFileName):
			ptr->setViewerFontFileName( newValue.asString() );
			FontInstance::setup(ptr->viewerFontFileName());
			break;
		case (PreferencesVariable::ForegroundColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::ForegroundColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::ForegroundColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::ForegroundColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::ForegroundColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::GlobeSize):
			ptr->setGlobeSize( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::GlyphDefaultColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::GlyphDefaultColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::GlyphDefaultColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::GlyphDefaultColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::GlyphDefaultColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::HBonds):
			ptr->setDrawHydrogenBonds( newValue.asBool() );
			break;
		case (PreferencesVariable::HBondDotRadius):
			ptr->setHydrogenBondDotRadius( newValue.asDouble() );
			break;
		case (PreferencesVariable::HDistance):
			ptr->setHydrogenDistance( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::ImageQuality):
			ptr->setImagePrimitiveQuality( newValue.asInteger(result) );
			updatePrimitives = true;
			break;
		case (PreferencesVariable::KeyAction):
			if (newValue.arraySize() == Prefs::nModifierKeys) for (n=0; n<Prefs::nModifierKeys; ++n)
			{
				ka = Prefs::keyAction( newValue.asString(n, result) );
				if ((ka != Prefs::nKeyActions) && result) ptr->setKeyAction( (Prefs::ModifierKey) n, ka);
				else { result = false; break; }
			}
			else if (hasArrayIndex)
			{
				ka = Prefs::keyAction( newValue.asString(result) );
				if ((ka != Prefs::nKeyActions) && result) ptr->setKeyAction( (Prefs::ModifierKey) (arrayIndex-1), ka);
				else result = false;
			}
			else
			{
				ka = Prefs::keyAction( newValue.asString(result) );
				if ((ka != Prefs::nKeyActions) && result) for (n=0; n<Prefs::nKeyActions; ++n) ptr->setKeyAction( (Prefs::ModifierKey) n, ka);
				else { result = false; break; }
			}
			break;
		case (PreferencesVariable::LabelSize):
			ptr->setLabelSize( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::LineAliasing):
			ptr->setLineAliasing( newValue.asBool() );
			break;
		case (PreferencesVariable::MaxCuboids):
			ptr->setMaxCuboids( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::MaxRings):
			ptr->setMaxRings( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::MaxRingSize):
			ptr->setMaxRingSize( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::MaxUndo):
			ptr->setMaxUndoLevels( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::MessagesFontSize):
			ptr->messagesFont().setPixelSize( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::MopacExe):
			ptr->setMopacExe( newValue.asString(result) );
			break;
		case (PreferencesVariable::MouseAction):
			if (newValue.arraySize() == Prefs::nModifierKeys) for (n=0; n<Prefs::nMouseActions; ++n)
			{
				ma = Prefs::mouseAction( newValue.asString(n, result) );
				if ((ma != Prefs::nMouseActions) && result) ptr->setMouseAction( (Prefs::MouseButton) n, ma);
				else { result = false; break; }
			}
			else if (hasArrayIndex)
			{
				ma = Prefs::mouseAction( newValue.asString(result) );
				if ((ma != Prefs::nMouseActions) && result) ptr->setMouseAction( (Prefs::MouseButton) (arrayIndex-1), ma);
				else result = false;
			}
			else
			{
				ma = Prefs::mouseAction( newValue.asString(result) );
				if ((ma != Prefs::nMouseActions) && result) for (n=0; n<Prefs::nMouseActions; ++n) ptr->setMouseAction( (Prefs::MouseButton) n, ma);
				else { result = false; break; }
			}
			break;
		case (PreferencesVariable::MouseMoveFilter):
			// Don't allow values below 1
			if (newValue.asInteger() > 0) ptr->setMouseMoveFilter( newValue.asInteger() );
			else Messenger::print("Values below 1 are not permitted for 'mousemovefilter' member.");
			break;
		case (PreferencesVariable::MultiSampling):
			ptr->setMultiSampling( newValue.asBool() );
			break;
		case (PreferencesVariable::NoQtSettings):
			ptr->setLoadQtSettings( newValue.asBool() );
			break;
		case (PreferencesVariable::PartitionGrid):
			if (newValue.arraySize() == 3) for (n=0; n<3; ++n) ptr->setPartitionGridSize(n, newValue.asInteger(n, result));
			else if (hasArrayIndex) ptr->setPartitionGridSize(arrayIndex-1, newValue.asInteger(result));
			else for (n=0; n<3; ++n) ptr->setPartitionGridSize(n, newValue.asInteger(result));
			break;
		case (PreferencesVariable::Perspective):
			ptr->setPerspective( newValue.asBool() );
			break;
		case (PreferencesVariable::PerspectiveFov):
			ptr->setPerspectiveFov( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::PolygonAliasing):
			ptr->setPolygonAliasing( newValue.asBool() );
			break;
		case (PreferencesVariable::Quality):
			ptr->setPrimitiveQuality( newValue.asInteger(result) );
			updatePrimitives = true;
			break;
		case (PreferencesVariable::ReuseQuality):
			ptr->setReusePrimitiveQuality( newValue.asBool() );
			break;
		case (PreferencesVariable::SelectionScale):
			ptr->setSelectionScale( newValue.asDouble(result) );
			updatePrimitives = true;
			break;
		case (PreferencesVariable::Shininess):
			ptr->setShininess( newValue.asInteger(result) );
			break;
		case (PreferencesVariable::SpecularColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::SpecularColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::SpecularColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::SpecularColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::SpecularColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::Spotlight):
			ptr->setSpotlightActive( newValue.asBool() );
			break;
		case (PreferencesVariable::SpotlightAmbient):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setSpotlightColour(Prefs::AmbientComponent, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setSpotlightColour(Prefs::AmbientComponent, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setSpotlightColour(Prefs::AmbientComponent, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setSpotlightColour(Prefs::AmbientComponent, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::SpotlightDiffuse):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setSpotlightColour(Prefs::DiffuseComponent, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setSpotlightColour(Prefs::DiffuseComponent, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setSpotlightColour(Prefs::DiffuseComponent, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setSpotlightColour(Prefs::DiffuseComponent, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::SpotlightPosition):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setSpotlightPosition(n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setSpotlightPosition(n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setSpotlightPosition(arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setSpotlightPosition(n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::SpotlightSpecular):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setSpotlightColour(Prefs::SpecularComponent, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setSpotlightColour(Prefs::SpecularComponent, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setSpotlightColour(Prefs::SpecularComponent, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setSpotlightColour(Prefs::SpecularComponent, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::StickNormalWidth):
			ptr->setStickLineNormalWidth(newValue.asDouble(result));
			break;
		case (PreferencesVariable::StickSelectedWidth):
			ptr->setStickLineSelectedWidth(newValue.asDouble(result));
			break;
		case (PreferencesVariable::TempDir):
			ptr->setTempDir( newValue.asString(result) );
			break;
		case (PreferencesVariable::UseWidgetForegroundBackground):
			ptr->setUseWidgetForegroundBackground( newValue.asBool() );
			break;
		case (PreferencesVariable::VdwCutoff):
			ptr->setVdwCutoff( newValue.asDouble(result) );
			break;
		case (PreferencesVariable::VibrationArrowColour):
			if (newValue.type() == VTypes::VectorData) for (n=0; n<3; ++n) ptr->setColour(Prefs::VibrationArrowColour, n, newValue.asVector(result)[n]);
			else if (newValue.arraySize() != -1) for (n=0; n<newValue.arraySize(); ++n) ptr->setColour(Prefs::VibrationArrowColour, n, newValue.asDouble(n, result));
			else if (hasArrayIndex) ptr->setColour(Prefs::VibrationArrowColour, arrayIndex-1, newValue.asDouble(result));
			else for (n=0; n<4; ++n) ptr->setColour(Prefs::VibrationArrowColour, n, newValue.asDouble(result));
			break;
		case (PreferencesVariable::ViewLock):
			vl = Prefs::viewLock( newValue.asString(result), true );
			if (vl != Prefs::nViewLockTypes) ptr->setViewLock(vl);
			else result = false;
			break;
		case (PreferencesVariable::ViewerFontFileName):
			ptr->setViewerFontFileName( newValue.asString(result) );
			break;
		case (PreferencesVariable::ViewRotationGlobe):
			ptr->setViewRotationGlobe( newValue.asBool() );
			break;
		case (PreferencesVariable::ZoomThrottle):
			ptr->setZoomThrottle( newValue.asDouble(result) );
			break;
		default:
			printf("PreferencesVariable::setAccessor doesn't know how to use member '%s'.\n", qPrintable(accessorData[acc].name));
			result = false;
			break;
	}

	// Update (low quality) rendering primitives?
	if (updatePrimitives) aten_->atenWindow()->ui.MainView->updatePrimitives(Viewer::LowQuality);

	Messenger::exit("PreferencesVariable::setAccessor");
	return result;
}

// Perform desired function
bool PreferencesVariable::performFunction(int i, ReturnValue& rv, TreeNode* node)
{
	Messenger::enter("PreferencesVariable::performFunction");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nFunctions))
	{
		printf("Internal Error: FunctionAccessor id %i is out of range for Preferences type.\n", i);
		Messenger::exit("PreferencesVariable::performFunction");
		return false;
	}

	// Get current data from ReturnValue
	bool result = true;
	Prefs* ptr = (Prefs*) rv.asPointer(VTypes::PreferencesData, result);
	if (result) switch (i)
	{
		default:
			printf("Internal Error: Access to function '%s' has not been defined in PreferencesVariable.\n", qPrintable(functionData[i].name));
			result = false;
			break;
	}
	Messenger::exit("PreferencesVariable::performFunction");
	return result;
}

