/*
    collection.cc: The Collection class and other related ones
    Copyright 2010 by Vincent Fourmond

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <headers.hh>
#include <collection.hh>

SerializationAccessor * CollectionDefinition::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  // ac->addAttribute("name", 
  // 		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("document-type", 
		   new SerializationQHash<DocumentDefinition>(&documentTypes));
  ac->addAttribute("ruby-class-code", 
		   &code);
  return ac;
}

void CollectionDefinition::dumpContents()
{
  QTextStream o(stdout);
  QHash<QString, DocumentDefinition>::const_iterator i = 
    documentTypes.constBegin();
  while (i != documentTypes.constEnd()) {
    o << "Type : " << i.value().name << endl;
    ++i;
  }
  o << "Code (" << code.name << "): " << code.code;
}

void CollectionDefinition::loadFromFile(QString fileName)
{
  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QXmlStreamReader w(&file);
  /// \todo should be factored out ?
  while(! w.isStartElement() && ! w.atEnd())
    w.readNext();
  readXML(&w);
}

Document * Collection::importFile(const QString & doctype, 
				  const QString & file)
{
  Document doc;
  /// \tdexception handle the exceptions raised ? (or do that in the
  /// calling functions ?)
  doc.attributes = definition->code.parseFileMetaData(doctype, file);
  doc.definition = &definition->documentTypes[doctype];
  doc.collection = this;
  documents.push_back(doc);
  return &documents.last();
}
