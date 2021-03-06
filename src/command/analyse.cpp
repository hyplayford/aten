/*
	*** Analysis Commands
	*** src/command/analyse.cpp
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

#include "command/commands.h"
#include "parser/commandnode.h"
#include "base/messenger.h"
#include "methods/rdf.h"
#include "methods/pdens.h"
#include "methods/geometry.h"
#include "model/bundle.h"
#include "model/model.h"

ATEN_USING_NAMESPACE

// Finalise calculated quantites ('finalise')
bool Commands::function_Finalise(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	for (Calculable* calc = obj.m->pendingQuantities.first(); calc != NULL; calc = calc->next) calc->finalise(obj.m);
	rv.reset();
	return true;
}

// Accumulate data for current frame ('frameanalyse')
bool Commands::function_FrameAnalyse(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	// Grab trajectory config for analysis
	Model* frame = obj.m->trajectoryCurrentFrame();
	for (Calculable* calc = obj.m->pendingQuantities.first(); calc != NULL; calc = calc->next) calc->accumulate(frame);
	rv.reset();
	return true;
}

// Calculate geometry ('geometry <name> <min> <binwidth> <nbins> <filename> <site1> <site2> [site3 [site4]]')
bool Commands::function_Geometric(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	Geometry* newGeom = new Geometry;
	obj.m->pendingQuantities.own(newGeom);

	// Set quantity name and destination filename
	newGeom->setName(c->argc(0));
	newGeom->setFilename(c->argc(4));

	// Associate sites to quantity
	newGeom->setSite(0, obj.m->findSite(c->argc(5)));
	newGeom->setSite(1, obj.m->findSite(c->argc(6)));
	if (c->hasArg(7)) newGeom->setSite(1, obj.m->findSite(c->argc(7)));
	if (c->hasArg(8)) newGeom->setSite(1, obj.m->findSite(c->argc(8)));
	newGeom->setRange(c->argd(1), c->argd(2), c->argi(3));

	rv.reset();
	return (newGeom->initialise());
}

// Accumulate data for current model ('modelanalyse')
bool Commands::function_ModelAnalyse(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	for (Calculable* calc = obj.m->pendingQuantities.first(); calc != NULL; calc = calc->next) calc->accumulate(obj.m);
	rv.reset();
	return true;
}

// Request calculation of a 3Ddens ('analyse pdens <name> <grid> <nsteps> <filename> <site1> <site2>')
bool Commands::function_PDens(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	Pdens* newPDens = new Pdens;
	obj.m->pendingQuantities.own(newPDens);

	// Set pdens name and destination filename
	newPDens->setName(c->argc(0));
	newPDens->setFilename(c->argc(3));

	// Associate sites to quantity
	newPDens->setSite(0, obj.m->findSite(c->argc(4)));
	newPDens->setSite(1, obj.m->findSite(c->argc(5)));
	newPDens->setRange(c->argd(1), c->argi(2));

	rv.reset();
	return (newPDens->initialise());
}

// Print current job list ('printjobs')
bool Commands::function_PrintJobs(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	return false;
}

// Request calculation of an RDF ('rdf <name> <rmin> <binwidth> <nbins> <filename> <site1> <site2>')
bool Commands::function_RDF(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	Rdf* newRdf = new Rdf;
	obj.m->pendingQuantities.own(newRdf);

	// Set RDF name and destination filename
	newRdf->setName(c->argc(0));
	newRdf->setFilename(c->argc(4));

	// Associate sites to quantity
	newRdf->setSite(0, obj.m->findSite(c->argc(5)));
	newRdf->setSite(1, obj.m->findSite(c->argc(6)));
	newRdf->setRange(c->argd(1), c->argd(2), c->argi(3));

	rv.reset();
	return (newRdf->initialise());
}

// Save calculated quantities to filenames provided ('savequantities')
bool Commands::function_SaveQuantities(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	for (Calculable* calc = obj.m->pendingQuantities.first(); calc != NULL; calc = calc->next) calc->save();
	rv.reset();
	return true;
}

// Calculate quantities over entire trajectory
bool Commands::function_TrajAnalyse(CommandNode* c, Bundle& obj, ReturnValue& rv)
{
	if (obj.notifyNull(Bundle::ModelPointer)) return false;
	int n, startframe, totalframes, frameskip, framestodo, framesdone;
	bool calculate;
	Model* frame;
	Calculable* calc;
	// Check that the model has a trajectory associated to it
	totalframes = obj.m->nTrajectoryFrames();
	if (totalframes == 0)
	{
		Messenger::print("No trajectory associated to model.");
		rv.reset();
		return false;
	}

	// Get start frame, frame skip, and frames to do (if supplied)
	startframe = c->argi(0);
	frameskip = c->argi(1);
	framestodo = (c->hasArg(2) ? c->argi(2) : -1);

	// Rewind trajectory to first frame and begin
	obj.m->seekFirstTrajectoryFrame();
	framesdone = 0;
	for (n=1; n <= totalframes; n++)
	{
		// Work out whether to calculate quantities from this frame
		calculate = true;
		if (n < startframe) calculate = false;
		else if ((n-startframe)%frameskip != 0) calculate = false;
		// Calculate quantities
		if (calculate)
		{
			frame = obj.m->trajectoryCurrentFrame();
			for (calc = obj.m->pendingQuantities.first(); calc != NULL; calc = calc->next) calc->accumulate(frame);
			framesdone ++;
		}

		// Check for required number of frames completed
		if (framesdone == framestodo) break;

		// Move to next frame
		if (n != totalframes) obj.m->seekNextTrajectoryFrame();
	}
	Messenger::print("Finished calculating properties - used %i frames from trajectory.", framesdone);

	rv.reset();
	return true;
}

