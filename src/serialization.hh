/**
    \file serializable.hh
    Infrastructure for data serialization (XML, Settings...)
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

#ifndef __SERIALIZATION_HH
#define __SERIALIZATION_HH

class Serializable;
template <class T> class WatchableList;

namespace Serialization {
  /// Reads tokens in the stream reader until a non-whitespace one is
  /// found.
  ///
  /// \todo The downside of this approach is that we can't save pure
  /// whitespace, unless it is written as CDATA. (then make writeXML
  /// write CDATA if non-empty whitespace and here check for CDATA...)
  void readNextToken(QXmlStreamReader * reader);
};

/// This class handles the very basic thing which we expect from a
/// serializable item: to be able to read and write itself from XML.
class SerializationAttribute {
public:

  ///\name XML-related functions
  ///
  /// Functions used to read/write attributes
  ///
  /// \todo maybe add the possibility for an attribute to be saved as
  /// an XML attribute and not as text ? (a virtual function)
  ///
  /// @{

  /// Writes the contents of the element to the QXmlStreamWriter
  virtual void writeXML(const QString & name, QXmlStreamWriter * writer) = 0;

  /// Reads from the given XML stream into the attributes. It assumes
  /// that the reader is at the startElement stuff.
  virtual void readXML(QXmlStreamReader * reader) = 0;

  /// Whether the given attribute could be written as XML attribute
  virtual bool isXMLAttribute() { return false;};

  /// Reads from String. Must be reimplemented if isXMLAttribute can
  /// return true.
  virtual void readFromString(const QString &) {
    return;
  };

public:

  /// @}

  virtual ~SerializationAttribute() { ;};

  /// Whether the SerializationAccessor should automatically free the
  /// attribute or not. For most cases, it should free it, but
  /// sometimes (when directly using children of Serializable),
  /// results could be catastrophic...
  virtual bool shouldBeDeleted() { return true;};

};

/// This class describes a single simple item (attribute) to be
/// serialized. Its implementation subclasses should probably use a
/// pointer to set/get data from QString/QVariant (but of course that
/// shouldn't happen in the base class, shouldn't it ?)
class SerializationItem : public SerializationAttribute {
protected:
  ///  Can be used as XML attribute
  bool isAttribute;
public:
  SerializationItem() : isAttribute(false) {;};

  /// Sets the value from a String
  virtual void setFromString(const QString & str);
  virtual void setFromVariant(const QVariant & v) = 0;

  /// Retrieve the value in the form of a string
  virtual QString valueToString();
  virtual QVariant valueToVariant() = 0;

  /// It is important to have a virtual destructor... (even if empty)
  virtual ~SerializationItem() {;};

  /// Writes the contents of the element to the QXmlStreamWriter
  virtual void writeXML(const QString & name, QXmlStreamWriter * writer);

  /// Reads from the given XML stream into the attributes. It assumes
  /// that the reader is at the startElement stuff.
  virtual void readXML(QXmlStreamReader * reader);

  virtual bool isXMLAttribute() { return isAttribute;};

  /// To deal well with XML attributes
  virtual void readFromString(const QString & str) {
    setFromString(str);
  };
};

class SerializationAccessor;

/// This abstract class describes a list of Serializable objects.
///
/// \todo Ideally, the use of accessors should be restricted to a
/// derived class.
///
/// 
class SerializationList : public SerializationAttribute {
public:

  /// \name Getter functions
  ///
  /// Functions to get information about the state of the list/dump
  /// its contents
  ///
  /// @{

  /// Returns the size of the list.
  virtual int listSize() = 0;

  /// Returns the SerializationAccessor element at nth element of the list
  virtual SerializationAccessor * accessorAt(int n) = 0;

  /// @}

  /// \name Setter functions
  ///
  /// Functions to change the state of the list.
  ///
  /// @{

  /// Simply augment the size of the list by one:
  virtual void augment() = 0;
  /// @}


  /// Writes the contents of the element to the QXmlStreamWriter
  virtual void writeXML(const QString & name, QXmlStreamWriter * writer);

  /// Reads from the given XML stream into the attributes. It assumes
  /// that the reader is at the startElement stuff. It only reads
  /// *one* element of the list, but that doesn't matter, as it can be
  /// called hundreds of times ;-)...
  virtual void readXML(QXmlStreamReader * reader);

  /// Write the numbered element to the stream
  virtual void writeXMLElement(int n, const QString & name, 
                               QXmlStreamWriter * writer);

  /// Reads the numbered element from stream
  virtual void readXMLElement(int n, QXmlStreamReader * reader);
};

/// This abstract class describes a QString-based hash of objects:
class SerializationHash : public SerializationAttribute {
protected:

  /// The name to be used as 'XML' attribute name for the key ?
  /// Careful, this key isn't saved ! The serialized stuff needs to
  /// take care of that !
  QString keyName;
public:

  /// \name Getter functions
  ///
  /// Functions to get information about the state of the list/dump
  /// its contents
  ///
  /// @{

  /// Returns the list of keys
  virtual QStringList keys() = 0;

  /// Returns the named element
  virtual Serializable * value(const QString & key) = 0;

  /// Creates an empty element with the named key
  virtual void newElement(const QString & key) = 0;

  /// @}

  /// \name Setter functions
  ///
  /// Functions to change the state of the list.
  ///
  /// @{

  /// Sets the named element
  virtual void insert(const QString & key, Serializable * el) = 0;

  /// @}

  /// Writes the element to the writer.
  ///
  /// \warning It does not write the key, the Serializable item must
  /// do it itself !!
  virtual void writeXML(const QString & name, QXmlStreamWriter * writer);

  /// Reads from the given XML stream into the attributes. It assumes
  /// that the reader is at the startElement stuff. It only reads
  /// *one* element of the hash.
  virtual void readXML(QXmlStreamReader * reader);

};


/// This class describes how to serialize a single serilization
/// element (a child of Serializable), that is which attributes are
/// available and how to set them.
///
/// \todo add callbacks (ie a way to call a function once values have
/// been set...), and maybe a target ?
///
/// \todo add the notion of "subitem", ie a subobject who is a
/// Serializable?
///
/// \todo find a way to add list of scalars ?
///
/// I think there's no way around the multiplication of base classes,
/// since it doesn't make sense to use
/// SerializationItem::setFromString on objects/lists/hashes...
class SerializationAccessor {
public:

  /// Generic pointer to the target ?
  Serializable * target;

  /// This hash holds all the attributes, whether they be simple
  /// objects or complex attributes.
  QHash<QString, SerializationAttribute *> attributes;

  /// Adds an attribute for this object.
  void addAttribute(QString name, SerializationAttribute * ser);

  /// Adds a scalar attribute using SerializationItemScalar<T>. The
  /// definition is in serialization-templates.hh
  template <class T> 
  void addScalarAttribute(const QString & name,
                          T * target,
                          bool isXMLAttribute = true);

  /// Adds an accessor-based attribute using
  /// SerializationItemAccessors<C>. The definition is in
  /// serialization-templates.hh
  template <class C> 
  void addAccessorsAttribute(const QString & name,
                             C * target,
                             void (C::*setter)(const QString &),
                             QString (C::*getter)() const,  
                             bool isXMLAttribute = true);

  /// Adds a list attribute using SerializationTemplateList<T>. The
  /// definition is in serialization-templates.hh
  template <class T> 
  void addListAttribute(const QString & name,
                        QList<T> * target);

  template <class T> 
  void addListAttribute(const QString & name,
                        WatchableList<T> * target);

  /// Adds a list attribute using SerializationScalarQList<T>. The
  /// definition is in serialization-templates.hh
  template <class T> 
  void addScalarListAttribute(const QString & name,
                              QList<T> * target, 
                              const QString & attrName);

  /// Adds a hash attribute using SerializationTemplateQHash<T>. The
  /// definition is in serialization-templates.hh
  template <class T> 
  void addHashAttribute(const QString & name,
                        QHash<QString, T> * target, 
                        const QString & keyName = "name");


  /// Creates a SerizalizationAccessor object for the given target.
  ///
  /// It is legal to specify NULL here. It just means that the
  /// SerializationAccessor is used to serialize something else than a
  /// child of Serializable. Hmmm, so long as it works ;-)...
  SerializationAccessor(Serializable * t);

  /// Frees the SerializationItem objects referenced by this object.
  ~SerializationAccessor();

  /// Function used for debugging: dump the values of the target to
  /// standard output.
  void dumpValues();

  /// Write to a QXmlStreamWriter. It assumes that the document has
  /// been started using QXmlStreamWriter::writeStartDocument.
  ///
  /// \p name is the element name as it should appear in the file.
  ///
  /// \todo XML attributes for certain tags.
  void writeXML(const QString & name, QXmlStreamWriter * writer);

  /// Read from a QXmlStreamReader and sets the target. This function
  /// assumes that the current token is the StartElement token whose
  /// name is the one given as second argument to writeXML.
  ///
  /// It is expected that the final state of the reader is the element
  /// *just after* the EndElement
  void readXML(QXmlStreamReader * reader);
};


// I don't like that too much, but it seems to be the way too go,
// turning serializable-templates.hh into some kind of "footer" rather
// than header ;-)...

#include <serializable-templates.hh>

#endif
