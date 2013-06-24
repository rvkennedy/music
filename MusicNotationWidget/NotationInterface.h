#ifndef NOTATION_INTERFACE_H
#define NOTATION_INTERFACE_H

class NotationInterface
{
	virtual void Fill(int first_bar,int num_bars,struct NotationSelection *n)=0;
};

#endif