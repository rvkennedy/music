#include "QMusicViewer.h"
#include "Connect.h"
#include "../MusicNotationWidget/MusicXMLDocument.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

MusicXMLDocument musicXMLDocument(NULL);
QMusicViewer::QMusicViewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	,MaxRecentFiles(8)
{
	ui.setupUi(this);
	ui.musicNotationWidget->setNotationInterface(&musicXMLDocument);
	
	readRecentFileList();
	loadLastEditedFile();
}

QMusicViewer::~QMusicViewer()
{

}

void QMusicViewer::on_actionOpen_triggered()
{
	QString filename=QFileDialog::getOpenFileName(this,tr("Open MusicXML file"), "", tr("MusicXML Files (*.xml);;All files (*.*)"));
	loadFile(filename);
}


void QMusicViewer::loadFile(QString filename)
{
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
	ui.musicNotationWidget->StructureChanged();
	ui.musicNotationWidget->update();
	setCurrentFile(filename);
}

void QMusicViewer::on_actionSave_triggered()
{
}

 void QMusicViewer::setCurrentFile(const QString &filename)
 {
	if (filename.isEmpty())
		setWindowTitle("QMusicViewer");
	else
		setWindowTitle(tr("QMusicViewer1: %2").arg(QFileInfo(filename).fileName()));

	if(filename.length()>0)
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope,"QMusicViewer", "Recent");
		QStringList files = settings.value("recentFileList").toStringList();
		files.removeAll(filename);
		files.prepend(filename);
		while (files.size() > MaxRecentFiles)
		    files.removeLast();
		settings.setValue("recentFileList", files);
		settings.setValue("currentFile", filename);
	}
	readRecentFileList();
 }

void QMusicViewer::loadLastEditedFile()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope,"QMusicViewer", "Recent");
	QString filename = settings.value("currentFile").toString();
	
	if(filename.contains(".xml"))
		loadFile(filename);
 }
void QMusicViewer::openRecentFile()
{
     QAction *action = qobject_cast<QAction *>(sender());
     if(action)
		loadFile(action->data().toString());
}

 void QMusicViewer::readRecentFileList()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope,"QMusicViewer", "Recent");
	QStringList files = settings.value("recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	while(ui.menuRecentFiles->actions().size())
	{
		QAction *a=ui.menuRecentFiles->actions()[0];
		QObject::disconnect(a,0,this,0);
		delete a;
	}
	for (int i = 0; i < numRecentFiles; ++i)
	{
        QAction *a=ui.menuRecentFiles->addAction("");
	    QString text = QFileInfo(files[i]).fileName();
	    a->setText(text);
	    a->setData(files[i]);
	    a->setVisible(true);
        CONNECT_AUTO(a,SIGNAL(triggered()),this, SLOT(openRecentFile()));
	}
}
 void QMusicViewer::updateRecentFileActions()
 {
 }
