#include "QMusicNotationWidget.h"
#include <QPainter>

void NotationSelectionForWidget::clear()
{
	notes.clear();
}
void NotationSelectionForWidget::addNote(int bar,int position,int octave,int step,float alter)
{
	Note n;
	n.bar		=bar;
	n.position	=position;
	n.octave	=octave;
	n.step		=step;
	n.alter		=alter;
	notes.append(n);
}

QMusicNotationWidget::QMusicNotationWidget(QWidget *parent)
	: QWidget(parent)
	,notationInterface(NULL)
{

}

QMusicNotationWidget::~QMusicNotationWidget()
{

}

void QMusicNotationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
	painter.setBrush(QBrush(Qt::white));
	painter.setPen(Qt::NoPen);
	QRect rect=this->geometry();
	rect.setTopLeft(QPoint(0,0));
	painter.drawRect(rect);

	painter.setPen(palette().dark().color());
	painter.drawLine(QLineF(0,0,5.f,5.f));
	QWidget::paintEvent(event);

	// if(notationInterface)
	if(notationInterface)
	{
	}
}