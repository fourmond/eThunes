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

/// Reads tokens in the stream reader until a non-whitespace one is
/// found.
///
/// \todo The downside of this approach is that we can't save pure
/// whitespace, unless it is written as CDATA. (then make writeXML
/// write CDATA if non-empty whitespace and here check for CDATA...)
inline void readNextToken(QXmlStreamReader * reader)
{
  do {
    reader->readNext();
  }
  while(reader->isWhitespace() && (! reader->atEnd()));
}

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
  readNextToken(reader);
  if(reader->isCharacters()) {
    setFromString(reader->text().toString());
    readNextToken(reader);
  }
  else if(reader->isEndElement()) {
    setFromString(QString());
  }
  else {
    fprintf(stderr, "Problem with reading attribute at line %ld\n",
	    (long) reader->lineNumber());
    return;
  }

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
  return new SerializationAccessor(this);
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
void SerializationAccessor::writeXML(const QString & name, 
				     QXmlStreamWriter * writer )
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
  /// \todo attributes parsing, if I come to that
  ///
  /// \todo Make it throw appropriate exceptions later on.
  if(target)
    target->prepareSerializationRead();
  while(!reader->atEnd()) {
    readNextToken(reader);

    if(reader->isEndElement()) {
      if(target)
	target->finishedSerializationRead();
      return;
    }

    if(! reader->isStartElement()) {
      fprintf(stderr, "We have trouble at line %ld\n", 
	      (long) reader->lineNumber());
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
  ac->writeXML(name, writer);
  delete ac;
}

void Serializable::readXML(QXmlStreamReader * reader)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->readXML(reader);
  delete ac;
}
