#ifndef QMUSICVIEWER_H
#define QMUSICVIEWER_H

#include <QtGui/QMainWindow>
#include "ui_QMusicViewer.h"

class QMusicViewer : public QMainWindow
{
	Q_OBJECT

public:
	QMusicViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QMusicViewer();
public slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
private:
	Ui::QMusicViewerClass ui;
};

#endif // QMUSICVIEWER_H
