#ifndef MUSICXMLDOCUMENT_H
#define MUSICXMLDOCUMENT_H

#include <QObject>
#include <QDomDocument>
#include <QtDesigner/QDesignerExportWidget>

#include <QVector>
#include <QString>
#include <QMap>
#include "NotationInterface.h"

namespace musicxml
{
	/*
    <measure number='5'>
      <attributes>
        <divisions>8</divisions>
      </attributes>
      <note>
        <pitch>
		<step>B</step>
		<alter>0.0</alter>
		<octave>3</octave>
		</pitch>
        <duration>4</duration>
        <voice>1</voice>
        <type>eighth</type>
        <staff>1</staff>
      </note>
	*/
	struct Pitch
	{
		int step;
		float alter;
		int octave;
	};
	enum NoteType
	{
		SIXTEENTH,EIGHTH,QUARTER,HALF,WHOLE
	};
	struct Note
	{
		bool rest;			// Either true or:
		Pitch pitch;
		int duration;
		int voice;
        NoteType type;		// e.g. eighth
		int staff;
	};
	struct Measure
	{
		int divisions;
		QVector<Note> notes;
	};
	struct Part
	{
		QString partName;							// part-name
		QMap<QString,QString> scoreInstruments;		// key: id, value: instrument-name
		QMap<QString,QString> midiInstruments;		// key: id, value: midi-channel
		QMap<int,Measure> measures;
	};
}

class QDESIGNER_WIDGET_EXPORT MusicXMLDocument : public QObject
	,public NotationInterface
{
	Q_OBJECT

public:
	MusicXMLDocument(QObject *parent);
	~MusicXMLDocument();

     void read(QIODevice *device);
     void write(QIODevice *device);

// NotationInterface
	void GetStructure(ScoreStructure *s);
	void Fill(int bar,NotationSelection *n);

	 QMap<QString,musicxml::Part> parts;
private slots:
//void updateDomElement(QTreeWidgetItem *item, int column);

 private:
     QDomDocument domDocument;
};

#endif // MUSICXMLDOCUMENT_H
