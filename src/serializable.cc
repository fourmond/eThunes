/*
    serializable.cc: Infrastructure for data serialization
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

#include <serializable.hh>

void SerializationItem::setFromString(const QString & v)
{
  setFromVariant(v);
}

QString SerializationItem::valueToString()
{
  return valueToVariant().toString();
}


SerializationAccessor * Serializable::serializationAccessor()
{
  return new SerializationAccessor;
}

SerializationAccessor::~SerializationAccessor()
{
  QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  while (i.hasNext()) {
     i.next();
     delete i.value();
  }
  simpleAttributes.clear();
  
}


void SerializationList::dumpAllValues()
{
  for(int i = 0; i < listSize(); i++) {
    SerializationAccessor * ac = at(i)->serializationAccessor();
    ac->dumpValues();
    delete ac;
  }
}

/// \todo raise exception when name already exists (in
/// simpleAttributes or elsewhere)
void SerializationAccessor::addSimpleAttribute(QString name, 
					       SerializationItem * ser)
{
  simpleAttributes[name] = ser;
}

/// \todo raise exception when name already exists.
void SerializationAccessor::addSerializableList(QString name, 
						SerializationList* ser)
{
  serializableLists[name] = ser;
}

void SerializationAccessor::dumpValues()
{
  QTextStream o(stdout);
  o << "Dumping values for object: " << target << endl;
  QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  while (i.hasNext()) {
    i.next();
    o << i.key() << " : " << i.value()->valueToString() << endl;
  }

  QHashIterator<QString, SerializationList *> j(serializableLists);
  while (j.hasNext()) {
    j.next();
    o << "List attribute: " << j.key() << endl;
    j.value()->dumpAllValues();
  }
}


// Implementation of the XML writing
void SerializationAccessor::writeXML(QXmlStreamWriter * writer, 
				     QString name)
{
  writer->writeStartElement(name);
  QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  while (i.hasNext()) {
    i.next();
    /// \todo put this function in SerializationItem
    writer->writeTextElement(i.key(), i.value()->valueToString());
  }

  QHashIterator<QString, SerializationList *> j(serializableLists);
  while (j.hasNext()) {
    j.next();
    /// \todo put this function in SerializationList
    for(int i = 0; i < j.value()->listSize();i++) {
      j.value()->at(i)->writeXML(writer, j.key());
    }
  }

  writer->writeEndElement();
}


// Implementation of the XML reading
void SerializationAccessor::readXML(QXmlStreamReader * reader)
{
  // Not that easy, I find.
  /// \todo attributes parsing, if I come to that
  while(!reader->atEnd()) {
    reader->readNext();
    // Fail on non-start element !
    

    /// \todo write the pendants of the write attributes as read attributes too.
  }

  // while (!xml.atEnd()) {
  //        xml.readNext();
  //        //... // do processing
  //  }
  //  if (xml.hasError()) {
  //        ... // do error handling
  //  } 
}


void Serializable::writeXML(QXmlStreamWriter * writer, QString name)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->writeXML(writer, name);
  delete ac;
}
