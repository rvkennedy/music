#ifndef MUSICNOTATIONWIDGETPLUGIN_H_H
#define MUSICNOTATIONWIDGETPLUGIN_H_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDesignerCustomWidgetInterface>

class MusicNotationWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
	MusicNotationWidgetPlugin(QObject *parent = 0);

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget(QWidget *parent);
	void initialize(QDesignerFormEditorInterface *core);

private:
	bool initialized;
};

#endif // MUSICNOTATIONWIDGETPLUGIN_H_H
