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


/// This class describes a single simple item (attribute) to be
/// serialized. Its implementation subclasses should probably use a
/// pointer to set/get data from QString/QVariant (but of course that
/// shouldn't happen in the base class, shouldn't it ?)
///
/// \todo Some information, such as:
/// 
/// \li can this information be a XML attribute or it should be a
/// full-blown node ? 
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


/// This class describes how to serialize a single serilization
/// element (a child of Serializable), that is which attributes are
/// available and how to set them.
///
/// \todo add callbacks (ie a way to call a function once values have
/// been set...), and maybe a target ?
class SerializationAccessor {
public:

  /// Generic pointer to the target.
  void * target;

  /// This hash holds simple attributes, to be set using
  /// addSimpleAttribute.
  QHash<QString, SerializationItem *> simpleAttributes;


  /// Adds a simple attribute for the object. The SerializationItem
  /// will be destroyed by SerializationAccessor.
  void addSimpleAttribute(QString name, SerializationItem * ser);


  /// Creates a SerizalizationAccessor object for the given target
  /// (not compulsory, at least for now).
  SerializationAccessor(void * t = NULL) { target = t;};

  /// Frees the SerializationItem objects referenced by this object.
  ~SerializationAccessor();

  /// Function used for debugging: dump the values of the target to
  /// standard output.
  void dumpValues();
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
  virtual SerializationAccessor * serializationAccessor();
};

#endif
