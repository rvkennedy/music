#include "QMusicNotationWidget.h"

#include <QtCore/QtPlugin>
#include "MusicNotationWidgetPlugin.h"


MusicNotationWidgetPlugin::MusicNotationWidgetPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void MusicNotationWidgetPlugin::initialize(QDesignerFormEditorInterface *)
{
	if (initialized)
		return;

	initialized = true;
}

bool MusicNotationWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget *MusicNotationWidgetPlugin::createWidget(QWidget *parent)
{
	return new QMusicNotationWidget(parent);
}

QString MusicNotationWidgetPlugin::name() const
{
	return "QMusicNotationWidget";
}

QString MusicNotationWidgetPlugin::group() const
{
	return "Roderick Kennedy";
}

QIcon MusicNotationWidgetPlugin::icon() const
{
	return QIcon();
}

QString MusicNotationWidgetPlugin::toolTip() const
{
	return QString();
}

QString MusicNotationWidgetPlugin::whatsThis() const
{
	return QString();
}

bool MusicNotationWidgetPlugin::isContainer() const
{
	return false;
}

QString MusicNotationWidgetPlugin::domXml() const
{
	return "<widget class=\"QMusicNotationWidget\" name=\"musicNotationWidget\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString MusicNotationWidgetPlugin::includeFile() const
{
	return "QMusicNotationWidget.h";
}

Q_EXPORT_PLUGIN2(MusicNotationWidget, MusicNotationWidgetPlugin)
