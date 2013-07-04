#ifndef QMUSICNOTATIONWIDGET_H
#define QMUSICNOTATIONWIDGET_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>
#include <QMap>
#include "NotationSelection.h"
#include "NotationInterface.h"
class QPainter;

namespace notation
{
	struct Clef
	{
		Clef():bar(0),sign(0),clef_line(0),centre_octave(0),centre_note(0){}
		int bar;
		char sign;			// 'G'=treble, 'F'=bass, 'C'=C clef
		int clef_line;		// Which line goes through the clef's "centre"
		int centre_octave;	// Where 4 is the octave starting on middle 'C'
		int centre_note;	// Measured in semitones, where C is 0
	};
	enum Mode
	{
		NONE=0,MAJOR,MINOR
	};
	struct KeySignature
	{
		int bar;
		int fifths;			// 0=C,1=G,2=D etc
		Mode mode;			// Major or minor
	};
	struct TimeSignature
	{
		int bar;
		int beats;			// e.g. 6
		int beat_type;		// e.g. 8, therefore 6/8 time.
	};
	struct Note
	{
		int bar;
		int clef;
		int position;
		int duration;
		int divisions;
		int octave;
		int step;
		float alter;
		int group;			// 0=none
	};
	struct Stem
	{
		bool up;		// if true, stem goes up. otherwise, down/
		QPointF start;
		QPointF end;	// end position, absolute.
	};
	struct RenderNote
	{
		QPointF centre;
		float height,width;
		Stem stem;
		bool isSolid,hasStem;
	};
	//! The left end of the centre line of the staff.
	struct Staff
	{
		QPointF origin;
		float dy;
		float width;
		float note_width;
		notation::Clef clef;
		notation::KeySignature key;
		notation::TimeSignature timeSignature;
	};
	struct Bar
	{
		float x;				// The horizontal position, accumulated from the widths of the previous bars.
		float width;			// The width, calculated from the contents of this bar across all the parts.
		QMap<int,int> order;	// A map from note position to order.
								// Each unique note position has an order value, 0,1,2,...
		float first_note_x;		// Offset from x to where the first note is.
	};
	struct ScorePart
	{
		QString name;
		QMap<int,notation::Clef> clefs;
		QMap<int,notation::KeySignature> keySignatures;
		QMap<int,notation::TimeSignature> timeSignatures;
		TimeSignature getTimeSignature(int bar) const;		// Last signature before or at this bar.
		KeySignature getKeySignature(int bar) const;
		float y;				// vertical position, affected by what's above it.
	};
	struct NotationSelectionForWidget:public NotationSelection
	{
		void clear();
		void addNote(const char *part_id,int bar,int clef,int position,int duration,int divisions,int octave,int step,float alter);
		struct Part
		{
			QVector<Note> notes;
		};
		QMap<QString,Part> parts;
		QVector<int> getBarNumbers();
		QVector<Note> getGroupNotes(QString part_id,int group) const;
	};

	struct ScoreStructureForWidget:public ScoreStructure
	{
		void clear();
		void setNumBars(int b);
		void setPart(const char *id,const char *n);
		void setClef(const char *id,int bar,int clef_number,const char *sign,int clef_line);
		void setKeySignature(const char *part_id,int bar,int fifths,const char *mode);
		void setTimeSignature(const char *part_id,int bar,int beats,int beat_type);
		QMap<QString,ScorePart> parts;
		int numBars;
	};
}
class QDESIGNER_WIDGET_EXPORT QMusicNotationWidget : public QWidget
{
	Q_OBJECT

public:
	QMusicNotationWidget(QWidget *parent = 0);
	~QMusicNotationWidget();

	void setNotationInterface(NotationInterface*n)
	{
		notationInterface=n;
		StructureChanged();
	}
public slots:
	void StructureChanged();
protected:
	void mouseMoveEvent ( QMouseEvent * event );
	void mousePressEvent ( QMouseEvent * event );
	void mouseReleaseEvent ( QMouseEvent * event );
	void mouseDoubleClickEvent( QMouseEvent * event );
	void wheelEvent ( QWheelEvent * event );

    virtual void paintEvent(QPaintEvent *event);
	QMap<QString,notation::ScorePart> parts;
	QVector<notation::Bar> bars;

	notation::Staff getStaff(const notation::ScorePart &part,int bar,int clef) const;
	notation::RenderNote getNoteCentre(QString part_id,const notation::Note &n) const;
	void calcBarWidth(int number);
	void groupNotes(notation::NotationSelectionForWidget &sel);

	void drawClef(QPainter &painter,QPointF pos,char sign);
	void drawKeySignature(QPainter &painter,QPointF pos,int fifths,int centre_note);
	void drawTimeSignature(QPainter &painter,QPointF pos,int beats,int beat_type);
	void drawStaff(QPainter &painter,const notation::Staff &staff);
	void drawBeam(QPainter &painter,QPointF pos1,QPointF pos2);
	void drawGroup(QPainter &painter,const QString &part_id,int group,const notation::NotationSelectionForWidget &sel);
	void drawNote(QPainter &painter,const QString &part_id,const notation::Note &n);
	void drawNote(QPainter &painter,notation::RenderNote renderNote);

	// This is where the widget gets its notes from.
	NotationInterface *notationInterface;
	// View properties
	QPointF WidgetToSheetPosition(QPointF wpos);
	QPointF originPos;
	QPointF last_pos;
	float xScale,yScale;
};


#endif // QMUSICNOTATIONWIDGET_H
