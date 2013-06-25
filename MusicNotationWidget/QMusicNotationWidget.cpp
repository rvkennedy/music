#include "QMusicNotationWidget.h"
#include <QPainter>
#include <math.h>

void NotationSelectionForWidget::clear()
{
	parts.clear();
}

void NotationSelectionForWidget::addNote(const char *part_id,int bar,int position,int duration,int divisions,int octave,int step,float alter)
{
	Part &p=parts[QString(part_id)];
	Note n;
	n.bar		=bar;
	n.position	=position;
	n.duration	=duration;
	n.divisions	=divisions;
	n.octave	=octave;
	n.order		=p.notes.size();
	n.step		=step;
	n.alter		=alter;
	p.notes.append(n);
}


void ScoreStructureForWidget::clear()
{
	numBars=0;
	parts.clear();
}
void ScoreStructureForWidget::setNumBars(int b)
{
	numBars=b;
}
void ScoreStructureForWidget::setPart(const char *id,const char *n)
{
	parts[id]=n;
}

QMusicNotationWidget::QMusicNotationWidget(QWidget *parent)
	: QWidget(parent)
	,notationInterface(NULL)
{
}

QMusicNotationWidget::~QMusicNotationWidget()
{
}

QMusicNotationWidget::Staff QMusicNotationWidget::getStaff(const QString &part_id,int bar)
{
	int index=parts[part_id].index;
	Staff staff;
	staff.note_width=4.f;
	staff.origin=QPointF(bars[bar].x,index*8.f+4.f);
	staff.width=bars[bar].width;
	staff.dy=1.f;
	return staff;
}

void QMusicNotationWidget::drawStaff(QPainter &painter,const QMusicNotationWidget::Staff &staff)
{
	painter.setPen(QPen(Qt::black));
	painter.setBrush(Qt::NoBrush);
	for(int i=-2;i<3;i++)
	{
		float y=staff.origin.y()+i*staff.dy;
		QPointF left(staff.origin.x(),y);
		QPointF right(left.x()+staff.width,y);
		painter.drawLine(left,right);
	}
	QPointF topleft(staff.origin.x(),staff.origin.y()-2*staff.dy);
	QPointF bottomright(topleft.x()+staff.width,staff.origin.y()+2*staff.dy);
	QRectF bounds(topleft,bottomright);
	painter.drawRect(bounds);
}
static float cosd(float r)
{
	return cos(r*3.1415926536f/180.f);
}
static float sind(float r)
{
	return sin(r*3.1415926536f/180.f);
}
// To draw a note, we need to find where its bar is, then position the note relative to the bar.

void QMusicNotationWidget::drawNote(QPainter &painter,const QString &part_id,const NotationSelectionForWidget::Note &n)
{
	Staff staff=getStaff(part_id,n.bar);
	
	float y_offset_from_centre=-n.step*0.5f+3.f;

	QPointF note_centre=staff.origin+QPointF(((float)n.order+1.f)*staff.note_width,(y_offset_from_centre)*staff.dy);

	static float rotation=-30.f;

	float height=staff.dy/2.f;
	float width=height*1.4f;
	QTransform t=painter.worldTransform();
	painter.translate(note_centre);
	painter.rotate(rotation);

	//
	bool solid=(n.duration<n.divisions*2);
	if(solid)
	{
		painter.setBrush(QBrush(Qt::black));
		painter.setPen(Qt::NoPen);
	}
	else
	{
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::black);
	}

	painter.drawEllipse(QPointF(0,0),width,height);
	painter.setWorldTransform(t);

	// Now draw the stem.
	if(n.duration<n.divisions*4)
	{
		// Is the note below the centreline?
		QPointF stem_start,stem_end;
		if(y_offset_from_centre>0)
		{
			stem_start=note_centre+width*QPointF(cosd(rotation),sind(rotation));
			stem_end=stem_start+QPointF(0,-3.f);
		}
		else
		{
			stem_start=note_centre-width*QPointF(cosd(rotation),sind(rotation));
			stem_end=stem_start+QPointF(0,3.f);
		}
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::black);
		painter.drawLine(stem_start,stem_end);
	}
}

float QMusicNotationWidget::calcBarWidth(int bar)
{
	NotationSelectionForWidget sel;
	notationInterface->Fill(bar,&sel);

	QMapIterator<QString,NotationSelectionForWidget::Part> i(sel.parts);
	float width=0.f;
	while(i.hasNext())
	{
		i.next();
		QString part_id=i.key();
		const NotationSelectionForWidget::Part &p=i.value();
		for(int j=0;j<p.notes.size();j++)
		{
			if(p.notes[j].order>width)
				width=p.notes[j].order;
		}
	}
	return width*4.f;
}

void QMusicNotationWidget::StructureChanged()
{
	ScoreStructureForWidget scoreStructureForWidget;
	notationInterface->GetStructure(&scoreStructureForWidget);
	bars.resize(scoreStructureForWidget.numBars);
	float x=0.f;
	for(int i=0;i<scoreStructureForWidget.numBars;i++)
	{
		bars[i].x=x;
		bars[i].width=calcBarWidth(i);
		x+=bars[i].width;
	}
	QMapIterator <QString,QString> i(scoreStructureForWidget.parts);
	int index=0;
	while(i.hasNext())
	{
		i.next();
		ScorePart &p=parts[i.key()];
		p.index=index++;
		p.name=i.value();
	}
}

void QMusicNotationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);
	// Get the window geometry:
	QRect rect=this->geometry();
	rect.setTopLeft(QPoint(0,0));

	painter.setBrush(QBrush(Qt::white));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect);

	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::black);
	QMapIterator<QString,ScorePart> i(parts);
	while(i.hasNext())
	{
		i.next();
		QString part_id=i.key();
		const ScorePart &scorePart=i.value();
		scorePart.index;
		QRect rect(0,scorePart.index*8*8+4+8,64,16);
		painter.drawText(rect,Qt::AlignRight,scorePart.name);
	}

	QRegion r(64,0,rect.width()-64,rect.height());
	painter.setClipRegion(r);
	painter.setClipping(true);

	painter.scale(8.f,8.f);
	//painter.translate(0.f,-rect.height());

	painter.setPen(Qt::black);
	painter.drawLine(QLineF(0,0,5.f,5.f));
	QWidget::paintEvent(event);

	if(notationInterface)
	{
		int bar0=0;
		int num_bars=4;
		for(int b=bar0;b<=bar0+num_bars;b++)
		{
			NotationSelectionForWidget sel;
			notationInterface->Fill(b,&sel);
			QMapIterator<QString,NotationSelectionForWidget::Part> i(sel.parts);
			while(i.hasNext())
			{
				i.next();
				QString part_id=i.key();
				const NotationSelectionForWidget::Part &p=i.value();
				Staff staff=getStaff(part_id,b);
				drawStaff(painter,staff);
				for(int i=0;i<p.notes.size();i++)
				{
					const NotationSelectionForWidget::Note &n=p.notes[i];
					drawNote(painter,part_id,n);
				}
			}
		}
	}
}