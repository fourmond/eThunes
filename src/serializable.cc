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

void SerializationItem::writeXML(const QString & name, 
				 QXmlStreamWriter * writer)
{
  writer->writeTextElement(name, valueToString());
}

void SerializationItem::readXML(QXmlStreamReader * reader)
{
  reader->readNext();
  if(! reader->isCharacters()) {
    fprintf(stderr, "Problem with reading attribute\n");
    return;
  }
  setFromString(reader->text().toString());
  reader->readNext();
  if(! reader->isEndElement()) {
    fprintf(stderr, "Expecting end\n");
    return;
  }
}


void SerializationList::readXML(QXmlStreamReader * reader)
{
  augment();
  at(listSize() - 1)->readXML(reader);
}

void SerializationList::writeXML(const QString & name, 
				 QXmlStreamWriter * writer)
{
  for(int i = 0; i < listSize();i++) {
    at(i)->writeXML(name, writer);
  }
}


SerializationAccessor * Serializable::serializationAccessor()
{
  return new SerializationAccessor;
}

SerializationAccessor::~SerializationAccessor()
{
  QHashIterator<QString, SerializationAttribute *> i(attributes);
  while (i.hasNext()) {
     i.next();
     if(i.value()->shouldBeDeleted())
       delete i.value();
  }
  attributes.clear();
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
void SerializationAccessor::addAttribute(QString name, 
					 SerializationAttribute * ser)
{
  attributes[name] = ser;
}


void SerializationAccessor::dumpValues()
{
  QTextStream o(stdout);
  o << "It is currently unsupported to dump to stdout" << endl;
  // o << "Dumping values for object: " << target << endl;
  // QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  // while (i.hasNext()) {
  //   i.next();
  //   o << i.key() << " : " << i.value()->valueToString() << endl;
  // }

  // QHashIterator<QString, SerializationList *> j(serializableLists);
  // while (j.hasNext()) {
  //   j.next();
  //   o << "List attribute: " << j.key() << endl;
  //   j.value()->dumpAllValues();
  // }
}


// Implementation of the XML writing
void SerializationAccessor::writeXML(QXmlStreamWriter * writer, 
				     QString name)
{
  writer->writeStartElement(name);
  QHashIterator<QString, SerializationAttribute *> i(attributes);
  while (i.hasNext()) {
    i.next();
    i.value()->writeXML(i.key(), writer);
  }

  writer->writeEndElement();
}


// Implementation of the XML reading
void SerializationAccessor::readXML(QXmlStreamReader * reader)
{
  // Not that easy, I find.
  /// \todo attributes parsing, if I come to that
  ///
  /// \todo Make it throw appropriate exceptions later on.
  while(!reader->atEnd()) {
    reader->readNext();
    if(! reader->isStartElement()) {
      fprintf(stderr, "We have trouble !\n");
      return;
    }
    
    // Now, find the elements.
    SerializationAttribute * attr = 
      attributes.value(reader->name().toString(), 0);
    if(! attr) {
      fprintf(stderr, "Unkown attribute: %s !\n", 
	      (const char*)reader->name().toString().toLocal8Bit());
      return;
    }
    // So dreadfully simple !!!
    attr->readXML(reader);

  }

}


void Serializable::writeXML(const QString & name, QXmlStreamWriter * writer)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->writeXML(writer, name);
  delete ac;
}

void Serializable::readXML(QXmlStreamReader * reader)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->readXML(reader);
  delete ac;
}
