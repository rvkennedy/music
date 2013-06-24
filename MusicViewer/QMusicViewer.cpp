#include "QMusicViewer.h"
#include "../MusicNotationWidget/MusicXMLDocument.h"
#include <QFileDialog>
#include <QMessageBox>

MusicXMLDocument musicXMLDocument(NULL);
QMusicViewer::QMusicViewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ui.musicNotationWidget->setNotationInterface(&musicXMLDocument);
}

QMusicViewer::~QMusicViewer()
{

}

void QMusicViewer::on_actionOpen_triggered()
{
	QString filename=QFileDialog::getOpenFileName(this,tr("Open MusicXML file"), "", tr("MusicXML Files (*.xml);;All files (*.*)"));
	QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
	{
         QMessageBox::warning(this, tr("MusicXML"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(filename)
                              .arg(file.errorString()));
         return;
     }
	musicXMLDocument.read(&file);
	ui.musicNotationWidget->update();
}

void QMusicViewer::on_actionSave_triggered()
{
}