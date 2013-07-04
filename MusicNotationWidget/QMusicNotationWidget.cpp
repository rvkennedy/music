#include "QMusicNotationWidget.h"
#include <QPainter>
#include <math.h>
#include <QMouseEvent>

using namespace notation;

static float cosd(float r)
{
	return cos(r*3.1415926536f/180.f);
}
static float sind(float r)
{
	return sin(r*3.1415926536f/180.f);
}

TimeSignature ScorePart::getTimeSignature(int bar) const
{
	int which_bar=-1;
	foreach(int b,timeSignatures.keys())
	{
		if(b>bar)
			break;
		which_bar=b;
	}
	return timeSignatures[which_bar];
}

KeySignature ScorePart::getKeySignature(int bar) const
{
	int which_bar=-1;
	foreach(int b,keySignatures.keys())
	{
		if(b>bar)
			break;
		which_bar=b;
	}
	return keySignatures[which_bar];
}

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
	n.group		=0;
	p.notes.append(n);
}

QVector<int> NotationSelectionForWidget::getBarNumbers()
{
	QSet<int> barnums;
	foreach(const Part &p,parts)
	{
		foreach(const Note &n,p.notes)
		{
			barnums.insert(n.bar);
		}
	}
	return barnums.toList().toVector();
}

QVector<Note> NotationSelectionForWidget::getGroupNotes(QString part_id,int group) const
{
	QVector<Note> group_notes;
	const Part &p=parts[part_id];
	foreach(const Note &n,p.notes)
	{
		if(n.group!=group)
			continue;
		group_notes.append(n);
	}
	return group_notes;
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
	notation::Clef &clef		=parts[part_id].clefs[clef_number];
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

void ScoreStructureForWidget::setKeySignature(const char *part_id,int bar,int fifths,const char *mode)
{
	notation::KeySignature &key		=parts[part_id].keySignatures[bar];
	key.bar			=bar;
	key.fifths		=fifths;
	if(strcmp(mode,"major")==0)
		key.mode	=notation::MAJOR;
	else
		key.mode	=notation::MINOR;
}

void ScoreStructureForWidget::setTimeSignature(const char *part_id,int bar,int beats,int beat_type)
{
	notation::TimeSignature &time		=parts[part_id].timeSignatures[bar];
	time.bar		=bar;
	time.beats		=beats;
	time.beat_type	=beat_type;
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

Staff QMusicNotationWidget::getStaff(const ScorePart &part,int bar,int clef) const
{
	Staff staff;
	staff.note_width	=4.f;
	staff.origin		=QPointF(bars[bar].x,part.y+((clef-1)*8.f+4.f));
	staff.width			=bars[bar].width;
	staff.dy			=1.f;
	if(part.clefs.contains(bar))
		staff.clef=part.clefs[bar];
	else
		staff.clef.sign=0;
	if(part.keySignatures.contains(bar))
		staff.key=part.keySignatures[bar];
	else
		staff.key.mode=notation::NONE;
	if(part.timeSignatures.contains(bar))
		staff.timeSignature=part.timeSignatures[bar];
	else
		staff.timeSignature.beats=0;
	return staff;
}

void QMusicNotationWidget::drawClef(QPainter &painter,QPointF pos,char sign)
{
	painter.setFont(QFont("NWCV15",4));
	pos.setY(pos.y()+2.f);
	if(sign=='G')
		painter.drawText(pos,"a");
	if(sign=='F')
		painter.drawText(pos,"b");
	if(sign=='C')
		painter.drawText(pos,"c");
}

void QMusicNotationWidget::drawKeySignature(QPainter &painter,QPointF pos,int fifths,int centre_note)
{
	painter.setFont(QFont("NWCV15",4));
	float X=pos.x();
	float Y=pos.y();
	if(fifths>=1) // F#
		painter.drawText(QPointF(X		,Y-2.f),"d");
	if(fifths>=2) // C#
		painter.drawText(QPointF(X+1.f	,Y-0.5f),"d");
	if(fifths>=3) // G#
		painter.drawText(QPointF(X+2.f	,Y-2.5f),"d");
	if(fifths>=4) // D#
		painter.drawText(QPointF(X+3.f	,Y-1.0f),"d");
	if(fifths>=5) // A#
		painter.drawText(QPointF(X+4.f	,Y+0.5f),"d");
	if(fifths>=6) // E#
		painter.drawText(QPointF(X+5.f	,Y-1.5f),"d");
	
	if(fifths<=-1) // Bb
		painter.drawText(QPointF(X		,Y-0.0f),"f");
	if(fifths<=-2) // Cb				 
		painter.drawText(QPointF(X+0.5f	,Y-1.5f),"f");
	if(fifths<=-3) // Gb				 
		painter.drawText(QPointF(X+1.f	,Y+0.5f),"f");
	if(fifths<=-4) // Db				 
		painter.drawText(QPointF(X+1.5f	,Y-1.0f),"f");
	if(fifths<=-5) // Ab				 
		painter.drawText(QPointF(X+2.f	,Y+1.0f),"f");
	if(fifths<=-6) // Eb				 
		painter.drawText(QPointF(X+2.5f	,Y+0.5f),"f");
}

void QMusicNotationWidget::drawTimeSignature(QPainter &painter,QPointF pos,int beats,int beat_type)
{
	if(beats==0||beat_type==0)
		return;
	painter.setFont(QFont("NWCV15",4));
	painter.drawText(pos.x(),pos.y(),QString("%1").arg(beats));
	painter.drawText(pos.x(),pos.y()+2.f,QString("%1").arg(beat_type));
}

void QMusicNotationWidget::drawStaff(QPainter &painter,const Staff &staff)
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

	int offset=1;
	if(staff.clef.sign!=0)
	{
		QPointF clefPos(staff.origin.x()+offset,staff.origin.y());
		drawClef(painter,clefPos,staff.clef.sign);
		offset+=4;
	}
	if(staff.key.mode!=0)
	{
		QPointF pos(staff.origin.x()+offset,staff.origin.y());
		drawKeySignature(painter,pos,staff.key.fifths,staff.clef.centre_note);
		offset+=4;
	}
	if(staff.timeSignature.beats!=0)
	{
		QPointF pos(staff.origin.x()+offset,staff.origin.y());
		drawTimeSignature(painter,pos,staff.timeSignature.beats,staff.timeSignature.beat_type);
		offset+=4;
	}
}

void QMusicNotationWidget::drawBeam(QPainter &painter,QPointF pos1,QPointF pos2)
{
	painter.setBrush(QBrush(Qt::black));
	painter.setPen(Qt::black);
	QPointF points[4];
	points[0]=points[1]=pos1;
	points[2]=points[3]=pos2;
	points[1]+=QPointF(0,0.25f);
	points[2]+=QPointF(0,0.25f);
	points[0]-=QPointF(0,0.25f);
	points[3]-=QPointF(0,0.25f);
	painter.drawPolygon(points, 4);
}

void QMusicNotationWidget::drawGroup(QPainter &painter,const QString &part_id,int group,const notation::NotationSelectionForWidget &sel)
{
	const ScorePart &part=parts[part_id];
	QVector<Note> notes=sel.getGroupNotes(part_id,group);
	QVector<RenderNote> renderNotes;
	foreach(Note n,notes)
	{
		RenderNote rn=getNoteCentre(part_id,n);
		renderNotes.append(rn);
	}
	bool mostAbove=false;
	int num_above=0;
	foreach(RenderNote rn,renderNotes)
	{
		if(rn.stem.end.y()>rn.stem.start.y())
			num_above++;
	}
	if(num_above>renderNotes.size()/2)
		mostAbove=true;
	for(int i=0;i<renderNotes.size();i++)
	{
		RenderNote &rn=renderNotes[i];
		if((!mostAbove&&rn.stem.end.y()>rn.stem.start.y())
			||(mostAbove&&rn.stem.end.y()<rn.stem.start.y()))
		{
			rn.stem.end=rn.stem.start+rn.stem.start-rn.stem.end;
			QPointF offset=rn.stem.start-rn.centre;
			rn.stem.start-=2.f*offset;
			rn.stem.end-=2.f*offset;
		}
	}
	// Draw the join between start and end stems.
	RenderNote rn0=renderNotes[0];
	RenderNote rn1=renderNotes[renderNotes.size()-1];
	drawBeam(painter,rn0.stem.end,rn1.stem.end);
	// Now render the notes, and join them to the beam.
	for(int i=0;i<renderNotes.size();i++)
	{
		RenderNote &rn=renderNotes[i];
		float prop=(rn.stem.end.x()-rn0.stem.end.x())/(rn1.stem.end.x()-rn0.stem.end.x());
		rn.stem.end=rn0.stem.end+prop*(rn1.stem.end-rn0.stem.end);
		drawNote(painter,rn);
	}
}

	static float rotation=-30.f;
// To draw a note, we need to find where its bar is, then position the note relative to the bar.
RenderNote QMusicNotationWidget::getNoteCentre(QString part_id,const Note &n) const
{
	RenderNote renderNote;
	const ScorePart &part=parts[part_id];
	Staff staff=getStaff(part,n.bar,n.clef);
	notation::Clef clef=part.clefs[n.clef];
	// The staff has an origin at the left of its centre line.
	// The clef defines both a symbol ('G', 'F', etc, and a line number).
	int offset_from_centre=n.step-clef.centre_note+7*(n.octave-clef.centre_octave);

	float y_offset_from_centre=-offset_from_centre*0.5f;
	const Bar &bar	=bars[n.bar];
	int order		=bar.order[n.position];
	renderNote.centre=staff.origin+QPointF(((float)order)*staff.note_width+bar.first_note_x,(y_offset_from_centre)*staff.dy);
	
	renderNote.height=staff.dy/2.f;
	renderNote.width=renderNote.height*1.4f;
	if(y_offset_from_centre>0)
	{
		renderNote.stem.start	=renderNote.centre+renderNote.width*QPointF(cosd(rotation),sind(rotation));
		renderNote.stem.end		=renderNote.stem.start+QPointF(0,-3.f);
	}
	else
	{
		renderNote.stem.start	=renderNote.centre-renderNote.width*QPointF(cosd(rotation),sind(rotation));
		renderNote.stem.end		=renderNote.stem.start+QPointF(0,3.f);
	}
	renderNote.isSolid=(n.duration<n.divisions*2);
	renderNote.hasStem=(n.duration<n.divisions*4);
	return renderNote;
}

void QMusicNotationWidget::drawNote(QPainter &painter,const QString &part_id,const Note &n)
{
	RenderNote renderNote=getNoteCentre(part_id,n);
	drawNote(painter,renderNote);
}

void QMusicNotationWidget::drawNote(QPainter &painter,RenderNote renderNote)
{
	QTransform t=painter.worldTransform();
	painter.translate(renderNote.centre);
	painter.rotate(rotation);
	//
	if(renderNote.isSolid)
	{
		painter.setBrush(QBrush(Qt::black));
		painter.setPen(Qt::NoPen);
	}
	else
	{
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::black);
	}
	painter.drawEllipse(QPointF(0,0),renderNote.width,renderNote.height);
	painter.setWorldTransform(t);
	// Now draw the stem.
	if(renderNote.hasStem)
	{
		// Is the note below the centreline?
		
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::black);
		painter.drawLine(renderNote.stem.start,renderNote.stem.end);
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
	bar.first_note_x=4.f;

	int space_for_clef		=0;
	int space_for_key		=0;
	int space_for_timesig	=0;
	// ok, but what if we need space on the left for clefs, key sigs or time sigs?
	foreach(const ScorePart &part,parts)
	{
		foreach(const Clef &clef,part.clefs)
		{
			if(clef.bar==b)
				space_for_clef=1;
		}
		foreach(const KeySignature &key,part.keySignatures)
		{
			if(key.bar==b)
				space_for_key=1;
		}
		foreach(const TimeSignature &time,part.timeSignatures)
		{
			if(time.bar==b)
				space_for_timesig=1;
		}
	}
	bar.first_note_x+=4.f*(space_for_clef+space_for_key+space_for_timesig);
	bar.width=bar.order.size()*4.f+bar.first_note_x;
}

// For each part in the selection, for each bar and staff, group the notes together.
void QMusicNotationWidget::groupNotes(NotationSelectionForWidget &sel)
{
	int group=1;
	QMap<int,int> groupSizes;
	// Try to group notes by beat.
	foreach(QString part_id,sel.parts.keys())
	{
		NotationSelectionForWidget::Part &p=sel.parts[part_id];
		const ScorePart &sp=parts[part_id];
		// Which bars?
		QSet<int> barnums;
		foreach(const Note &n,p.notes)
			barnums.insert(n.bar);
		foreach(int b,barnums)
		{
			const TimeSignature &time=sp.getTimeSignature(b);
			const Bar &bar=bars[b];
			QSet<int> clefs;
			foreach(const Note &n,p.notes)
				if(n.bar==b)
					clefs.insert(n.clef);
			// Now group for each clef:
			foreach(int c,clefs)
			{
				Staff staff=getStaff(sp,b,c);
				QVector<int> notes_in_bar;
				for(int i=0;i<p.notes.size();i++)
				{
					Note &n=p.notes[i];
					if(n.bar!=b||n.clef!=c)
						continue;
					notes_in_bar.append(i);
					// if durations>=divisions, it's a crotchet or longer.
					if(n.duration>=n.divisions)
						continue;
					int beat=n.position/n.divisions;
					n.group=group+beat;
				}
	// But exceptions to the beat rule: If it's 4:4 time, and the first two or last two beats are
	// all quavers, they are grouped together.
	// Assuming that the notes are in order:
				bool all_quavers[]={true,true,true,true};
				if(staff.timeSignature.beat_type==4&&staff.timeSignature.beats==4)
				foreach(int i,notes_in_bar)
				{
					Note &n=p.notes[i];
					int beat=n.position/n.divisions;
					// If it's a quaver.
					if(n.divisions/n.duration!=2)
						all_quavers[beat]=false;
				}
				// First two beats all quavers?
				if(all_quavers[0]&&all_quavers[1])
				{
					foreach(int i,notes_in_bar)
					{
						Note &n=p.notes[i];
						int beat=n.position/n.divisions;
						if(beat<2)
							n.group=group+0;
					}
				}
				if(all_quavers[2]&&all_quavers[3])
				{
					foreach(int i,notes_in_bar)
					{
						Note &n=p.notes[i];
						int beat=n.position/n.divisions;
						if(beat>=2)
							n.group=group+2;
					}
				}
				group+=time.beats;
			}
		}
		
		foreach(const Note &n,p.notes)
		{
			if(!groupSizes.contains(n.group))
				groupSizes[n.group]=0;
			groupSizes[n.group]++;
		}
		for(int i=0;i<p.notes.size();i++)
		{
			Note &n=p.notes[i];
			if(groupSizes[n.group]==1)
				n.group=0;
		}
	}
}

void QMusicNotationWidget::StructureChanged()
{
	ScoreStructureForWidget scoreStructureForWidget;
	notationInterface->GetStructure(&scoreStructureForWidget);
	parts=scoreStructureForWidget.parts;
	bars.resize(scoreStructureForWidget.numBars);
	float x=2.f;
	for(int i=0;i<scoreStructureForWidget.numBars;i++)
	{
		bars[i].x=x;
		calcBarWidth(i);
		x+=bars[i].width;
	}
	QMapIterator <QString,ScorePart> i(scoreStructureForWidget.parts);
	int index=0;
	float y=0.f;
	while(i.hasNext())
	{
		i.next();
		ScorePart p=i.value();
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
	QMapIterator<QString,ScorePart> i(parts);
	while(i.hasNext())
	{
		i.next();
		QString part_id=i.key();
		const ScorePart &scorePart=i.value();

		QRect rect(0,(scorePart.y+2-originPos.y())*yScale,64,4*yScale);
		painter.drawText(rect,Qt::AlignRight|Qt::AlignVCenter,scorePart.name);
	}

	QRegion r(64,0,rect.width()-64,rect.height());
	painter.setClipRegion(r);
	painter.setClipping(true);

	painter.translate(32,0);
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
			groupNotes(sel);
			QMapIterator<QString,NotationSelectionForWidget::Part> i(sel.parts);
			while(i.hasNext())
			{
				i.next();
				QString part_id=i.key();
				const NotationSelectionForWidget::Part &p=i.value();
				QMapIterator<int,notation::Clef> j(parts[part_id].clefs);
				while(j.hasNext())
				{
					j.next();
					const Staff &staff=getStaff(parts[part_id],b,j.key());
					drawStaff(painter,staff);
				}
				QSet<int> groups;
				for(int i=0;i<p.notes.size();i++)
				{
					const Note &n=p.notes[i];
					if(n.group==0)
						drawNote(painter,part_id,n);
					else groups.insert(n.group);
				}
				foreach(int i,groups)
				{
					drawGroup(painter,part_id,i,sel);
				}
			}
		}
	}
}