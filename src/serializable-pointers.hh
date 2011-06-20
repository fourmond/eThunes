/**
    \file serializable-pointers.hh
    Template classes for serializing objects pointed to in a type-safe way
    Copyright 2011 by Vincent Fourmond

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

#ifndef __SERIALIZABLE_POINTERS_HH
#define __SERIALIZABLE_POINTERS_HH

#include <serializable.hh>

/// This template class provides serialization for pointers to
/// classes. For type-safe serialization, the base class must provide:
/// \li a static createObject(const QString & name) function
/// \li a QString typeName() const function
/// \li of course, the target class should be a child of Serializable
///
/// \todo Maybe this class could integrate a fallback in case of a
/// missing plugin ?
template <class T>
class SerializationItemPointer : public SerializationAttribute {
  T ** target;                  // Pointer to pointer
public:
  SerializationItemPointer<T>(T ** t) {
    target = t;
  };

  virtual bool isXMLAttribute() const {
    return false;
  };

  virtual void readXML(QXmlStreamReader *reader) {
    // We are at the startElement, so we read the type = attribute,
    // then we proceed !

    if(! reader->isStartElement()) {
      fprintf(stderr, "Pointer serialization problem at line %ld: we should be at a start element, instead of %s (name = %s)\n",
              (long) reader->lineNumber(),
              (const char*) reader->tokenString().toLocal8Bit(),
              (const char*) reader->name().toString().toLocal8Bit()
              );
      return;
    }
    const QXmlStreamAttributes & attributes =  reader->attributes();
    if(attributes.hasAttribute("typename")) {
      *target = T::createObject(attributes.value("typename").toString());
      if(*target) {
        Serialization::readNextToken(reader);
        (*target)->readXML(reader);
      }
      else {
        fprintf(stderr, "For some reason, couldn't create target type\n");
      }
    }
    else {
      fprintf(stderr, "Didn't find a typename attribute at line %ld\n",
              (long) reader->lineNumber());

    }
    Serialization::readNextToken(reader);
    if(! reader->isEndElement())
      fprintf(stderr, "Hmm, we should be at end element now...\n");
  };

  virtual void writeXML(const QString & name, QXmlStreamWriter * writer) {
    writer->writeStartElement(name);
    writer->writeAttribute("typename", (*target)->typeName());
    (*target)->writeXML("data", writer);
    writer->writeEndElement();
  };
  

};

/// This class provides serialization for QList of objects that do not
/// inherit Serializable, but that can be serialized using
/// SerializationItemScalar
template <typename T>
class SerializationPointerQList : public SerializationList {
  QList<T*> * target;
public:
  SerializationPointerQList(QList<T*> * t) :
    target(t) {;};

  virtual int listSize() { return target->size();};

  virtual void augment() {
    target->append(NULL);       // Append NULL by default
  };

  virtual SerializationAccessor * accessorAt(int) {
    return NULL;
  };

  virtual void writeXMLElement(int n, const QString & name, 
                               QXmlStreamWriter * writer) {
    SerializationItemPointer<T> it(&target->operator[](n));
    it.writeXML(name, writer);
  };

  virtual void readXMLElement(int n, QXmlStreamReader * reader) {
    SerializationItemPointer<T> it(&target->operator[](n));
    it.readXML(reader);
  }


};



#endif
