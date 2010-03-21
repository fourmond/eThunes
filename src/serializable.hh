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

#ifndef __SERIALIZABLE_HH
#define __SERIALIZABLE_HH

class Serializable;

/// This class describes a single simple item (attribute) to be
/// serialized. Its implementation subclasses should probably use a
/// pointer to set/get data from QString/QVariant (but of course that
/// shouldn't happen in the base class, shouldn't it ?)
///
/// \todo Some information, such as:
/// 
/// \li can this information be a XML attribute or it should be a
/// full-blown node ?
///
/// \todo it would be interesting to a have template accessor-based
/// serializers, using pointers to member functions, see
/// http://www.parashift.com/c++-faq-lite/pointers-to-members.html
///
/// This might lift the need for callbacks.
///
/// Meanwhile, callbacks could use "pointer-to-members" with a
/// template stuff.
class SerializationItem {
public:
  /// Sets the value from a String
  virtual void setFromString(const QString & str);
  virtual void setFromVariant(const QVariant & v) = 0;

  /// Retrieve the value in the form of a string
  virtual QString valueToString();
  virtual QVariant valueToVariant() = 0;

  /// It is important to have a virtual destructor... (even if empty)
  virtual ~SerializationItem() {;};
};


/// This template class class should be used for any reasonably
/// scalar.
template <typename T>
class SerializationItemScalar : public SerializationItem {
  T * target;
public:
  SerializationItemScalar<T>(T * t) {target = t;};
  virtual void setFromVariant(const QVariant & v) {
    *target = v.value<T>();
  };

  virtual QVariant valueToVariant() {
    return QVariant(*target);
  };
};

/// This abstract class describes a list of Serializable objects.
class SerializationList {
public:

  /// \name Getter functions
  ///
  /// Functions to get information about the state of the list/dump
  /// its contents
  ///
  /// @{

  /// Dump all the values in the list
  void dumpAllValues();
  
  /// Returns the size of the list.
  virtual int listSize() = 0;

  /// Returns the nth element of the list
  virtual Serializable * at(int n) = 0;
  
  /// @}

  /// \name Setter functions
  ///
  /// Functions to change the state of the list.
  ///
  /// @{

  /// Append the given element at the end of the list.
  virtual void append(Serializable * el) = 0;

  /// Simply augment the size of the list by one:
  virtual void augment() = 0;
  /// @}
  
};


/// This template class deals with the specific case of QList of
/// children of Serializable (but not pointers).
template <class T>
class SerializationQList : public SerializationList {
  QList<T> * target;
public:
  SerializationQList(QList<T> * t) { target = t;};
  
  virtual int listSize() { return target->size();};

  virtual Serializable * at(int n) { return &target->operator[](n);};

  virtual void append(Serializable * el) {
    target->append(*static_cast<T*>(el));
  };

  virtual void augment() { 
    target->append(T());
  };
  
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

  /// Generic pointer to the target.
  void * target;

  /// This hash holds simple attributes, to be set using
  /// addSimpleAttribute.
  QHash<QString, SerializationItem *> simpleAttributes;

  /// This hash contains the sublists of Serializable objects, to be
  /// set using addSerializableList.
  QHash<QString, SerializationList*> serializableLists;


  /// Adds a simple attribute for the object. The SerializationItem
  /// will be destroyed by SerializationAccessor.
  void addSimpleAttribute(QString name, SerializationItem * ser);

  /// Adds as an "attribute" an object describing a list of
  /// Serializable items.
  void addSerializableList(QString name, SerializationList* ser);


  /// Creates a SerizalizationAccessor object for the given target
  /// (not compulsory, at least for now).
  SerializationAccessor(void * t = NULL) { target = t;};

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
  void writeXML(QXmlStreamWriter * writer, QString name);

  /// Read from a QXmlStreamReader and sets the target. This function
  /// assumes that the current token is the StartElement token whose
  /// name is the one given as second argument to writeXML.
  void readXML(QXmlStreamReader * reader);
};

/// All classes that should be serialized at some point should include
/// this class in their ancestry. It shouldn't contain data.
class Serializable {
public:
  /// This function should be reimplemented in children, as it
  /// provides the way for the object to be serialized.
  ///
  /// A subclass will return an appropriate SerializationAccessor
  /// object to set/get its data using a unique interface.
  virtual SerializationAccessor * serializationAccessor() = 0;

  /// \name Saving/loading functions
  ///
  /// Functions to save/load Serializable objects from various
  /// sources.
  /// @{
  
  /// Writes the Serializable object into the writer as name.
  virtual void writeXML(QXmlStreamWriter * writer, QString name);
  
  /// @}
};

#endif
