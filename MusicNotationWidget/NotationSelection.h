#ifndef NOTATION_SELECTION_H
#define NOTATION_SELECTION_H

struct NotationSelection
{
	virtual void clear()=0;
	// Add a note to the selection, at the given octave where 4 indicates the octave started by middle C
	// and step(0 to 7), with C=0, plus an alteration in semitones (-1 = flat, 1=sharp)
	virtual void addNote(int bar,int positon,int octave,int step,float alter)=0;
};

#endif