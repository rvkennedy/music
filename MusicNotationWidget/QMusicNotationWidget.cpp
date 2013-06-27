#include "QMusicNotationWidget.h"
#include <QPainter>
#include <math.h>
#include <QMouseEvent>

void NotationSelectionForWidget::clear()
{
	parts.clear();
}

void NotationSelectionForWidget::addNote(const char *part_id,int bar,int clef,int position,int duration,int divisions,int octave,int step,float alter)
{
	Part &p=parts[QString(part_id)];
	Note n;
	n.bar		=bar;
	n.clef		=clef;
	n.position	=position;
	n.duration	=duration;
	n.divisions	=divisions;
	n.octave	=octave;
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
	parts[id].name=n;
	parts[id].y=0.f;
}

void ScoreStructureForWidget::setClef(const char *part_id,int bar,int clef_number,const char *sign,int clef_line)
{
	NotationSelection::Clef &clef		=parts[part_id].clefs[clef_number];
	clef.bar		=bar;
	clef.sign		=sign[0];			// 'G'=treble, 'F'=bass, 'C'=C clef
	clef.clef_line	=clef_line;
	int clef_note=0;
	int clef_note_octave=0;
	if(clef.sign			=='G')
	{
		clef_note_octave	=4;
		clef_note			=4;//'G'
	}
	else if(clef.sign		=='F')
	{
		clef_note_octave	=3;
		clef_note			=3;//'F'
	}
	else if(clef.sign		=='C')
	{
		clef_note_octave	=4;
		clef_note			=0;//'C'
	}
	// But what if the clef line is not the expected one?
	clef.centre_note		=clef_note+2*(3-clef_line);
	clef.centre_octave	=clef_note_octave;
	while(clef.centre_note<0)
	{
		clef.centre_note+=7;
		clef.centre_octave--;
	}
	while(clef.centre_note>6)
	{
		clef.centre_note-=7;
		clef.centre_octave++;
	}
}

QMusicNotationWidget::QMusicNotationWidget(QWidget *parent)
	: QWidget(parent)
	,notationInterface(NULL)
	,originPos(0.f,0.f)
	,xScale(8.f)
	,yScale(8.f)
{
}

QMusicNotationWidget::~QMusicNotationWidget()
{
}

QMusicNotationWidget::Staff QMusicNotationWidget::getStaff(const ScoreStructureForWidget::ScorePart &part,int bar,int clef)
{
	//int index=part.index;
	Staff staff;
	staff.note_width=4.f;
	staff.origin=QPointF(bars[bar].x,part.y+clef*8.f+4.f);
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
	const ScoreStructureForWidget::ScorePart &part=parts[part_id];
	Staff staff=getStaff(part,n.bar,n.clef);
	NotationSelection::Clef clef=part.clefs[n.clef];
	// The staff has an origin at the left of its centre line.
	// The clef defines both a symbol ('G', 'F', etc, and a line number).
	int offset_from_centre=n.step-clef.centre_note+7*(n.octave-clef.centre_octave);

	float y_offset_from_centre=-offset_from_centre*0.5f;
	int order=bars[n.bar].order[n.position];
	QPointF note_centre=staff.origin+QPointF(((float)order+1.f)*staff.note_width,(y_offset_from_centre)*staff.dy);
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

void QMusicNotationWidget::calcBarWidth(int b)
{
	NotationSelectionForWidget sel;
	Bar &bar=bars[b];
	notationInterface->Fill(b,&sel);

	QMapIterator<QString,NotationSelectionForWidget::Part> i(sel.parts);
	QSet<int> positions;
	float width=0.f;
	while(i.hasNext())
	{
		i.next();
		QString part_id=i.key();
		const NotationSelectionForWidget::Part &p=i.value();
		for(int j=0;j<p.notes.size();j++)
		{
			positions.insert(p.notes[j].position);
		}
	}
	QList<int> pos_list=positions.toList();
	qSort(pos_list);
	for(int j=0;j<pos_list.size();j++)
	{
		bar.order[pos_list[j]]=j;
	}
	bar.width=(bar.order.size()+2.f)*4.f;
}

void QMusicNotationWidget::StructureChanged()
{
	ScoreStructureForWidget scoreStructureForWidget;
	notationInterface->GetStructure(&scoreStructureForWidget);
	bars.resize(scoreStructureForWidget.numBars);
	float x=2.f;
	for(int i=0;i<scoreStructureForWidget.numBars;i++)
	{
		bars[i].x=x;
		calcBarWidth(i);
		x+=bars[i].width;
	}
	parts=scoreStructureForWidget.parts;
	QMapIterator <QString,ScoreStructureForWidget::ScorePart> i(scoreStructureForWidget.parts);
	int index=0;
	float y=0.f;
	while(i.hasNext())
	{
		i.next();
		ScoreStructureForWidget::ScorePart p=i.value();
		p.y=y;
		parts[i.key()]=p;
		float height=8.f*p.clefs.size();
		y+=height;
	}
}

static bool ControlPressed(QMouseEvent *e)
{
	return e->modifiers()&Qt::ControlModifier;
}

static bool ShiftPressed(QMouseEvent *e)
{
	return e->modifiers()&Qt::ShiftModifier;
}

QPointF QMusicNotationWidget::WidgetToSheetPosition(QPointF wpos)
{
	QPointF spos(wpos.x()/xScale,wpos.y()/yScale);
	return spos;
}

void QMusicNotationWidget::mouseMoveEvent(QMouseEvent * e )
{
	QPointF pos=WidgetToSheetPosition(e->pos());
	Qt::MouseButtons b=e->buttons();
	if(b!=0)
	{
		QPointF diff=last_pos-pos;
		originPos+=diff;
		if(originPos.x()<0.f)
			originPos.setX(0.f);
		if(originPos.y()<0.f)
			originPos.setY(0.f);
		update();
	}
	last_pos=pos;
}

void QMusicNotationWidget::mousePressEvent(QMouseEvent * e )
{
	QPointF pos=WidgetToSheetPosition(e->pos());
	last_pos=pos;
}

void QMusicNotationWidget::mouseReleaseEvent(QMouseEvent * e )
{
}

void QMusicNotationWidget::mouseDoubleClickEvent( QMouseEvent * e )
{
}

void QMusicNotationWidget::wheelEvent(QWheelEvent * event )
{
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
	QMapIterator<QString,ScoreStructureForWidget::ScorePart> i(parts);
	while(i.hasNext())
	{
		i.next();
		QString part_id=i.key();
		const ScoreStructureForWidget::ScorePart &scorePart=i.value();
		QRect rect(0,(scorePart.y-originPos.y())*yScale+4+8,64,16);
		painter.drawText(rect,Qt::AlignRight,scorePart.name);
	}

	QRegion r(64,0,rect.width()-64,rect.height());
	painter.setClipRegion(r);
	painter.setClipping(true);

	painter.translate(64,0);
	painter.scale(xScale,yScale);

	painter.translate(-originPos);

	painter.setPen(Qt::black);
	QWidget::paintEvent(event);

	if(notationInterface)
	{
		float x0=originPos.x();
		float width_units=rect.width()/xScale;
		int bar0=-1;
		int num_bars=4;
		for(int i=0;i<bars.size();i++)
		{
			Bar &b=bars[i];
			if(bar0<0&&b.x+b.width>=originPos.x())
				bar0=i;
			if(bar0>=0&&b.x>originPos.x()+width_units)
			{
				num_bars=i-bar0;
				break;
			}
		}
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
				QMapIterator<int,NotationSelection::Clef> j(parts[part_id].clefs);
				while(j.hasNext())
				{
					j.next();
					const Staff &staff=getStaff(parts[part_id],b,j.key());
					drawStaff(painter,staff);
				}
				for(int i=0;i<p.notes.size();i++)
				{
					const NotationSelectionForWidget::Note &n=p.notes[i];
					drawNote(painter,part_id,n);
				}
			}
		}
	}
}