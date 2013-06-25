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
	void addNote(const char *part_id,int bar,int position,int duration,int divisions,int octave,int step,float alter);
	struct Note
	{
		int bar;
		int position;
		int duration;
		int divisions;
		int order;		// 0,1,2,3,...
		int octave;
		int step;
		float alter;
	};
	struct Part
	{
		QVector<Note> notes;
	};
	QMap<QString,Part> parts;
};

struct ScoreStructureForWidget:public ScoreStructure
{
	void clear();
	void setNumBars(int b);
	void setPart(const char *id,const char *n);
	QMap<QString,QString> parts;
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
    virtual void paintEvent(QPaintEvent *event);
	//! The left end of the centre line of the staff.
	struct Staff
	{
		QPointF origin;
		float dy;
		float width;
		float note_width;
	};
	struct ScorePart
	{
		QString name;
		int index;
	};
	struct Bar
	{
		float x;		// The horizontal position, accumulated from the widths of the previous bars.
		float width;	// The width, calculated from the contents of this bar across all the parts.
	};

	Staff getStaff(const QString &part_id,int bar);
	void drawStaff(class QPainter &painter,const Staff &staff);
	void drawNote(class QPainter &painter,const QString &part_id,const NotationSelectionForWidget::Note &n);
	// This is where the widget gets its notes from.
	NotationInterface *notationInterface;
	QMap<QString,ScorePart> parts;
	// The list of bars, each of which has its own width.
	QVector<Bar> bars;
	float calcBarWidth(int number);
};

#endif // QMUSICNOTATIONWIDGET_H
