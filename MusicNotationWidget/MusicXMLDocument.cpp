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
			 QDomElement note=measure.firstChildElement("note");
			 while (!note.isNull())
			 {
				 QDomElement note	=part.firstChildElement("note");

				 note				= note.nextSiblingElement("note");
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

void MusicXMLDocument::Fill(int first_bar,int num_bars,NotationSelection *n)
{
	n->clear();
}