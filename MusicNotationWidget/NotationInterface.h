#ifndef NOTATION_INTERFACE_H
#define NOTATION_INTERFACE_H

class NotationInterface
{
public:
	virtual void GetStructure(struct ScoreStructure *s)=0;
	virtual void Fill(int bar,struct NotationSelection *n)=0;
};

#endif