#ifndef QMUSICNOTATIONWIDGET_H
#define QMUSICNOTATIONWIDGET_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>
#include "NotationSelection.h"
#include "NotationInterface.h"

struct NotationSelectionForWidget:public NotationSelection
{
	void clear();
	void addNote(int bar,int positon,int octave,int step,float alter);
	struct Note
	{
		int bar;
		int position;
		int octave;
		int step;
		float alter;
	};
	QVector<Note> notes;
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
	}
protected:
    virtual void paintEvent(QPaintEvent *event);

	// This is where the widget gets its notes from.
	NotationInterface *notationInterface;
};

#endif // QMUSICNOTATIONWIDGET_H
