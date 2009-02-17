/*
	*** Model selection functions
	*** src/model/selection.cpp
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

#include "model/model.h"
#include "model/undoevent.h"
#include "model/undostate.h"
#include "base/elements.h"

// Return the number of selected atoms
int Model::nSelected()
{
	return nSelected_;
}

// Return the number of marked atoms
int Model::nMarked()
{
	return nMarked_;
}

// Mark all atoms in model
void Model::markAll()
{
	for (Atom *i = atoms_.first(); i != NULL; i = i->next) i->setSelected(TRUE,TRUE);
	nMarked_ = atoms_.nItems();
	msg.print(Messenger::Verbose, "All atoms marked.\n");
}

// Match marked atoms to current selection
void Model::markSelectedAtoms()
{
	for (Atom *i = atoms_.first(); i != NULL; i = i->next) i->isSelected() ? selectAtom(i, TRUE) : deselectAtom(i, TRUE);
	msg.print(Messenger::Verbose, "There are now %i atoms marked.\n", nMarked_);
}

// Move specified atom up in the list (to lower ID)
void Model::shiftAtomUp(Atom *i)
{
	msg.enter("Model::shiftAtomUp");
	if (i == NULL)
	{
		printf("NULL Atom pointer passed to shiftAtomDown.");
		msg.exit("Model::shiftAtomUp");
		return;
	}
	int tempid, oldid;
	oldid = i->id();
	// Shift atom up
	atoms_.shiftUp(i);
	// Swap atomids with the new 'next' atom
	tempid = i->next->id();
	i->next->setId(oldid);
	i->setId(tempid);
	// Add the change to the undo state (if there is one)
	if (recordingState_ != NULL)
	{
		IdShiftEvent *newchange = new IdShiftEvent;
		newchange->set(oldid, -1);
		recordingState_->addEvent(newchange);
	}
	changeLog.add(Log::Structure);
	msg.exit("Model::shiftAtomUp");
}

// Move specified atom up in the list (to higher ID)
void Model::shiftAtomDown(Atom *i)
{
	msg.enter("Model::shiftAtomDown");
	if (i == NULL)
	{
		printf("NULL Atom pointer passed to shiftAtomDown.");
		msg.exit("Model::shiftAtomDown");
		return;
	}
	int tempid, oldid;
	oldid = i->id();
	// Shift atom down
	atoms_.shiftDown(i);
	// Swap atomids with the new 'next' atom
	tempid = i->prev->id();
	i->prev->setId(oldid);
	i->setId(tempid);
	// Add the change to the undo state (if there is one)
	if (recordingState_ != NULL)
	{
		IdShiftEvent *newchange = new IdShiftEvent;
		newchange->set(oldid, 1);
		recordingState_->addEvent(newchange);
	}
	changeLog.add(Log::Structure);
	msg.exit("Model::shiftAtomDown");
}

// Move atoms 'up'
void Model::shiftSelectionUp()
{
	msg.enter("Model::shiftSelectionUp");
	if (nSelected_ == 0)
	{
		msg.print("No atoms selected.");
		msg.exit("Model::shiftSelectionUp");
		return;
	}
	Atom *i, *next;
	// For each selected atom in the model, shift it one place 'up' the atom list
	i = atoms_.first()->next;
	while (i != NULL)
	{
		next = i->next;
		if (i->isSelected() && (i != atoms_.first())) shiftAtomUp(i);
		i = next;
	}
	changeLog.add(Log::Structure);
	msg.exit("Model::shiftSelectionUp");
}

// Move atoms 'down'
void Model::shiftSelectionDown()
{
	msg.enter("Model::shiftSelectionDown");
	if (nSelected_ == 0)
	{
		msg.print("No atoms selected.");
		msg.exit("Model::shiftSelectionDown");
		return;
	}
	Atom *i, *next;
	//for (n=0; n<atoms.nItems(); n++)
	// For each selected atom in the model, shift it one place 'down' the atom list
	i = atoms_.last()->prev;
	while (i != NULL)
	{
		next = i->prev;
		if (i->isSelected()) shiftAtomDown(i);
		i = next;
	}
	changeLog.add(Log::Structure);
	msg.exit("Model::shiftSelectionDown");
}

// Move atoms to start
void Model::moveSelectionToStart()
{
	msg.enter("Model::moveSelectionToStart");
	int n;
	Atom *next, *i;
	// For each selected atom in the model, shift it to the end of the list
	i = atoms_.last();
	for (n=0; n<atoms_.nItems(); n++)
	{
		next = i->prev;
		if (i->isSelected()) atoms_.moveToStart(i);
		i = next;
	}
	// Renumber atoms
	renumberAtoms();
	changeLog.add(Log::Structure);
	msg.exit("Model::moveSelectionToStart");
}

// Move atoms to end
void Model::moveSelectionToEnd()
{
	msg.enter("Model::moveSelectionToEnd");
	int n;
	Atom *next, *i;
	// For each selected atom in the model, shift it to the end of the list
	i = atoms_.first();
	for (n=0; n<atoms_.nItems(); n++)
	{
		next = i->next;
		if (i->isSelected()) atoms_.moveToEnd(i);
		i = next;
	}
	// Renumber atoms
	renumberAtoms();
	changeLog.add(Log::Structure);
	msg.exit("Model::moveSelectionToEnd");
}

// Get selection cog
Vec3<double> Model::selectionCog()
{
	Vec3<double> result;
	Atom *first = firstSelected();
	if (first != NULL)
	{
		for (Atom *i = first; i != NULL; i = i->nextSelected()) result += cell_.mim(i,first);
		result /= nSelected_;
	}
	return result;
}

// Get selection com
Vec3<double> Model::selectionCom()
{
	Vec3<double> result;
	double massnorm = 0.0;
	Atom *first = firstSelected();
	if (first != NULL)
	{
		for (Atom *i = first->nextSelected(); i != NULL; i = i->nextSelected())
		{
			result += cell_.mim(i,first) * elements().atomicMass(i);
			massnorm += elements().atomicMass(i);
			if (i->element() == 0)
			{
				msg.print("Warning - selection contains an unknown element - mass assumed to be 1.0\n");
				massnorm += 1.0;
			}
		}
		result /= massnorm;
	}
	return result;
}

// Set selection visibility
void Model::selectionSetHidden(bool hidden)
{
	for (Atom *i = firstSelected(); i != NULL; i = i->nextSelected()) setHidden(i, hidden);
	changeLog.add(Log::Visual);
}

// Fix selected atom positions
void Model::selectionSetFixed()
{
	// Sets 'fixed' values to TRUE
	for (Atom *i = atoms_.first(); i != NULL; i = i->next) if (i->isSelected()) i->setPositionFixed(TRUE);
}

// Free selected atom positions
void Model::selectionSetFree()
{
	// Sets 'fixed' values to TRUE
	for (Atom *i = atoms_.first(); i != NULL; i = i->next) if (i->isSelected()) i->setPositionFixed(FALSE);
}

// Select bound and selected atoms from the current atom
void Model::fragmentFromSelectionSelector(Atom *i, Reflist<Atom,int> &list)
{
	Atom *j;
	for (Refitem<Bond,int> *bref = i->bonds(); bref != NULL; bref = bref->next)
	{
		j = bref->item->partner(i);
		if (j->isSelected())
		{
			deselectAtom(j);
			list.add(j);
			fragmentFromSelectionSelector(j, list);
		}
	}
}

// Get atoms of a bound fragment with the current selection
void Model::fragmentFromSelection(Atom *start, Reflist<Atom,int> &list)
{
	msg.enter("Model::fragmentFromSelection");
	if ((start == NULL) || (!start->isSelected()))
	{
		msg.print("No atom provided, or atom is not selected.");
		msg.exit("Model::fragmentFromSelection");
		return;
	}
	// Clear the provided list and add the start atom
	list.clear();
	list.add(start);
	// From the atom provided, put all bound and selected atoms in the reflist provided
	deselectAtom(start);
	fragmentFromSelectionSelector(start, list);
	msg.exit("Model::fragmentFromSelection");
}

// Reorder bound atoms/fragments within the selection so that they are consecutive
void Model::reorderSelectedAtoms()
{
	msg.enter("Model::reorderSelectedAtoms");
	// First, mark atoms from current selection
	markSelectedAtoms();
	// Loop over marked atoms in ID order. For each move any bound neighbour that is not adjacent to the index above this one.
	Atom *i, *j;
	Refitem<Bond,int> *rb;
	int diff, n;
	for (i = firstSelected(TRUE); i != NULL; i = i->nextSelected(TRUE))
	{
		// Loop over bonds
		for (rb = i->bonds(); rb != NULL; rb = rb->next)
		{
			j = rb->item->partner(i);
			if (!j->isSelected(TRUE)) continue;
			diff = j->id() - i->id();
			for (n=1; n<diff; n++) shiftAtomUp(j);
		}
		// De-mark the root atom
		deselectAtom(i, TRUE);
	}
	msg.exit("Model::reorderSelectedAtoms");
}