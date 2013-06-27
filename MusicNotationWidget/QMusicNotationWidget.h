#ifndef QMUSICNOTATIONWIDGET_H
#define QMUSICNOTATIONWIDGET_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>
#include <QMap>
#include "NotationSelection.h"
#include "NotationInterface.h"

struct NotationSelectionForWidget:public NotationSelection
{
	void clear();
	void addNote(const char *part_id,int bar,int clef,int position,int duration,int divisions,int octave,int step,float alter);
	struct Part
	{
		QVector<Note> notes;
	};
	QMap<QString,Part> parts;
};

struct ScoreStructureForWidget:public ScoreStructure
{
	struct ScorePart
	{
		QString name;
		//int index;
		QMap<int,NotationSelection::Clef> clefs;
		float y;				// vertical position, affected by what's above it.
	};
	void clear();
	void setNumBars(int b);
	void setPart(const char *id,const char *n);
	void setClef(const char *id,int bar,int clef_number,const char *sign,int clef_line);
	QMap<QString,ScorePart> parts;
	int numBars;
};
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
	//! The left end of the centre line of the staff.
	struct Staff
	{
		QPointF origin;
		float dy;
		float width;
		float note_width;
	};
	struct Bar
	{
		float x;				// The horizontal position, accumulated from the widths of the previous bars.
		float width;			// The width, calculated from the contents of this bar across all the parts.
		QMap<int,int> order;	// A map from note position to order.
								// Each unique note position has an order value, 0,1,2,...
	};

	Staff getStaff(const ScoreStructureForWidget::ScorePart &part,int bar,int clef);
	void drawStaff(class QPainter &painter,const Staff &staff);
	void drawNote(class QPainter &painter,const QString &part_id,const NotationSelectionForWidget::Note &n);
	// This is where the widget gets its notes from.
	NotationInterface *notationInterface;
	QMap<QString,ScoreStructureForWidget::ScorePart> parts;
	// The list of bars, each of which has its own width.
	QVector<Bar> bars;
	void calcBarWidth(int number);
	QPointF WidgetToSheetPosition(QPointF wpos);
	QPointF originPos;
	QPointF last_pos;
	float xScale,yScale;
};


#endif // QMUSICNOTATIONWIDGET_H
