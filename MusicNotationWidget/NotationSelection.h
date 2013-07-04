#ifndef NOTATION_SELECTION_H
#define NOTATION_SELECTION_H

struct ScoreStructure
{
	virtual void clear()=0;
	virtual void setNumBars(int b)=0;
	virtual void setPart(const char *part_id,const char *name)=0;
	virtual void setClef(const char *part_id,int bar,int clef_number,const char *sign,int clef_line)=0;
	virtual void setKeySignature(const char *part_id,int bar,int fifths,const char *mode)=0;
	virtual void setTimeSignature(const char *part_id,int bar,int beats,int beat_type)=0;
};

struct NotationSelection
{
	virtual void clear()=0;
	// Add a note to the selection, at the given octave where 4 indicates the octave started by middle C
	// and step(0 to 7), with C=0, plus an alteration in semitones (-1 = flat, 1=sharp)

	// divisions is the demoninator of the fraction, where position and duration are the denominators.
	virtual void addNote(const char *part_id,int bar,int clef,int position,int duration,int divisions,int octave,int step,float alter)=0;
};

#endif