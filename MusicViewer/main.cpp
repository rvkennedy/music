#include "QMusicViewer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMusicViewer w;
	w.show();
	return a.exec();
}
