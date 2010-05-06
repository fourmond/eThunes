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

void Serialization::readNextToken(QXmlStreamReader * reader)
{
  do {
    reader->readNext();
  }
  while((reader->isWhitespace() || reader->isComment()) 
	&& (! reader->atEnd()));
}

using namespace Serialization;

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
  // We don't write if empty ? Seems reasonable ?
  /// \todo Maybe make that customizable using virtual functions again
  /// !
  QString value = valueToString();
  if(! value.isEmpty()) {
    if( isXMLAttribute()) 
      writer->writeAttribute(name, value);
    else
      writer->writeTextElement(name, value);
  }
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
  QStringList trueAttributes; // = XML attributes
  QStringList otherAttributes; // = more complex ones ?
  QHashIterator<QString, SerializationAttribute *> i(attributes);
  while (i.hasNext()) {
    i.next();
    if(i.value()->isXMLAttribute())
      trueAttributes << i.key();
    else
      otherAttributes << i.key();
  }

  for(int i = 0; i < trueAttributes.size(); i++)
    attributes.value(trueAttributes[i])->writeXML(trueAttributes[i], writer);
  for(int i = 0; i < otherAttributes.size(); i++)
    attributes.value(otherAttributes[i])->writeXML(otherAttributes[i], writer);

  writer->writeEndElement();
}


// Implementation of the XML reading
void SerializationAccessor::readXML(QXmlStreamReader * reader)
{
  /// \tdexception Make it throw appropriate exceptions later on.
  if(target)
    target->prepareSerializationRead();

  if(! reader->isStartElement()) {
    fprintf(stderr, "We have trouble at line %ld: we should be at a start element\n", 
	    (long) reader->lineNumber());
    return;
  }
  
  // First, we read the attributes
  {
    // This is a scope for the lazy guy...
    const QXmlStreamAttributes & attr =  reader->attributes();
    for(int i = 0; i < attr.size(); i++) {
      SerializationAttribute * a = 
	attributes.value(attr[i].name().toString(), 0);
	
      if(a && a->isXMLAttribute())
	a->readFromString(attr[i].value().toString());
      else 
	fprintf(stderr, "Unexpected XML attribute: '%s' = '%s' at line %ld %p\n", 
		(const char*) attr[i].name().toString().toLocal8Bit(),
		(const char*) attr[i].value().toString().toLocal8Bit(),
		(long) reader->lineNumber(), a);
    }
  }

  // Now, we read the inner part
  while(!reader->atEnd()) {
    readNextToken(reader);
    if(reader->isEndElement()) {
      if(target)
	target->finishedSerializationRead();
      return;
    }

    // Now, find the elements.
    SerializationAttribute * attr = 
      attributes.value(reader->name().toString(), 0);
    if(! attr) {
      /// \tdexception on unkown attributes !
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

/// A small reminder: this function is called at the start element
void SerializationHash::readXML(QXmlStreamReader * reader)
{
  const QXmlStreamAttributes & attr = reader->attributes();
  if(! attr.hasAttribute(keyName)) {
    fprintf(stderr, "Missing attribute %s for hash\n",
	    (const char*) keyName.toLocal8Bit());
    return;
  }
  QString key = attr.value(keyName).toString();
  newElement(key);
  value(key)->readXML(reader);
}

void SerializationHash::writeXML(const QString & name, 
				 QXmlStreamWriter * writer)
{
  QStringList k = keys();
  for(int i = 0; i < k.size();i++)
    value(k[i])->writeXML(name, writer);
}

void SerializationStringList::readXML(QXmlStreamReader * reader)
{
  target->append(reader->attributes().value(attributeName).toString());
  readNextToken(reader);
}

void SerializationStringList::writeXML(const QString & name, 
				       QXmlStreamWriter * writer)
{
  for(int i = 0; i < target->size();i++) {
    writer->writeStartElement(name);
    writer->writeAttribute(attributeName, target->value(i));
    writer->writeEndElement();
  }
}
