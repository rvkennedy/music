#include "MusicXMLDocument.h"
#include <iostream>
#include "NotationSelection.h"

MusicXMLDocument::MusicXMLDocument(QObject *parent)
	: QObject(parent)
{

}

MusicXMLDocument::~MusicXMLDocument()
{

}

void MusicXMLDocument::read(QIODevice *device)
{
     QString errorStr;
     int errorLine;
     int errorColumn;

     if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                 &errorColumn))
	 {
		 std::cerr<<"Parse error at line "<<errorLine<<", column "<<errorColumn<<": "<<
			 (const char*)errorStr.toAscii()<<std::endl;
         return ;
     }

     QDomElement root = domDocument.documentElement();
     if (root.tagName() != "score-partwise")
	 {
		 std::cerr<<"The file is not a MusicXML file.\n";
         return ;
     }

     //clear();

    // disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
    //            this, SLOT(updateDomElement(QTreeWidgetItem*,int)));
	 QDomElement partList=root.firstChildElement("part-list");
     if (!partList.isNull())
	 {
		QDomElement scorePart=partList.firstChildElement("score-part");
		while(!scorePart.isNull())
		{
			QString id			=scorePart.attribute("id");
			musicxml::Part &p	=parts[id];
			p.partName			=scorePart.firstChildElement("part-name").text();

			scorePart			=scorePart.nextSiblingElement("score-part");
		}
	 }
     QDomElement part = root.firstChildElement("part");
     while (!part.isNull())
	 {
		 QString id=part.attribute("id");
		 musicxml::Part &p=parts[id];
		 p.partName=part.attribute("part-name");
		 QDomElement measure=part.firstChildElement("measure");
		 while (!measure.isNull())
		 {
			 musicxml::Measure *m=&p.measures[measure.attribute("number").toInt()];
			 QDomElement note=measure.firstChildElement("note");
			 while (!note.isNull())
			 {
				// QDomElement note	=part.firstChildElement("note");
				 note				=note.nextSiblingElement("note");
			 }
			 measure = measure.nextSiblingElement("measure");
		 }
         part = part.nextSiblingElement("part");
     }
   //  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
    //         this, SLOT(updateDomElement(QTreeWidgetItem*,int)));

}

void MusicXMLDocument::write(QIODevice *device)
{
}

void MusicXMLDocument::GetStructure(ScoreStructure *s)
{
	s->clear();
    QDomElement root=domDocument.documentElement();
	
	QDomElement partList=root.firstChildElement("part-list");
	if (!partList.isNull())
	{
		QDomElement scorePart=partList.firstChildElement("score-part");
		while(!scorePart.isNull())
		{
			QString part_id		=scorePart.attribute("id");
			QString part_name	=scorePart.firstChildElement("part-name").text();
			s->setPart(part_id.toAscii(),part_name.toAscii());
			scorePart			=scorePart.nextSiblingElement("score-part");
		}
	}
	int numBars=0;
	QDomElement part=root.firstChildElement("part");
	while(!part.isNull())
	{
		QString part_id			=part.attribute("id");
		QDomElement measure		=part.firstChildElement("measure");
		while(!measure.isNull())
		{
			int b=measure.attribute("number").toInt();
			if(b>numBars)
				numBars=b;
			measure = measure.nextSiblingElement("measure");
		}
		part = part.nextSiblingElement("part");
	}
	s->setNumBars(numBars);
}

void MusicXMLDocument::Fill(int bar,NotationSelection *n)
{
	n->clear();
	QVector<QString> note_names;
	note_names<<"C"<<"D"<<"E"<<"F"<<"G"<<"A"<<"B";
    QDomElement root=domDocument.documentElement();
	QDomElement part=root.firstChildElement("part");
	while(!part.isNull())
	{
		QString part_id			=part.attribute("id");
		QDomElement measure	=part.firstChildElement("measure");
		while(!measure.isNull())
		{
			int b			=measure.attribute("number").toInt()-1;
			if(bar==b)
			{
				int divisions	=measure.firstChildElement("attributes").firstChildElement("divisions").text().toInt();
				int position	=0;
				QDomElement note=measure.firstChildElement("note");
				while (!note.isNull())
				{
					QDomElement pitch=note.firstChildElement("pitch");
					int step=0,octave=0;
					float alter=0.f;
					int duration=note.firstChildElement("duration").text().toInt();
					if(!pitch.isNull())
					{
						step=note_names.indexOf(pitch.firstChildElement("step").text());
						alter=pitch.firstChildElement("alter").text().toFloat();
						octave=pitch.firstChildElement("octave").text().toInt();
						n->addNote(part_id.toAscii(),bar,position,duration,divisions,octave,step,alter);
					}
					note=note.nextSiblingElement("note");
					position+=duration;
				}
			}
			measure = measure.nextSiblingElement("measure");
		}
		part = part.nextSiblingElement("part");
	}
}