/** 
    \file serializable-templates.hh
    Template classes for serialization
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

    \todo This file is seriously getting crowded; possibly splitting
    is in order?  
*/

#ifndef __SERIALIZABLE_TEMPLATES_HH
#define __SERIALIZABLE_TEMPLATES_HH

#ifndef CALL_MEMBER_FN
/// \todo this should be moved somewhere appropriate
#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember)) 
#endif

/// This template class class should be used for any reasonably
/// scalar.
template <typename T>
class SerializationItemScalar : public SerializationItem {
  T * target;
public:
  SerializationItemScalar<T>(T * t, bool attr = false) {
    target = t;
    isAttribute = attr;
  };
  virtual void setFromVariant(const QVariant & v) {
    *target = v.value<T>();
  };

  virtual QVariant valueToVariant() {
    return QVariant(*target);
  };
};



/// This template class handles the serialization of a QString based
/// on read/write accessors.
template <class C>
class SerializationItemAccessors : public SerializationItem {
  typedef void (C::*Setter)(const QString &);
  Setter setter;
  typedef QString (C::*Getter)();
  Getter getter;

  C * targetClass;
public:
  SerializationItemAccessors<C>(C * t, Setter s, Getter g, bool attr = false) {
    targetClass = t;
    setter = s;
    getter = g;
    isAttribute = attr;
  };

  virtual void setFromVariant(const QVariant & v) {
    CALL_MEMBER_FN(targetClass, setter)(v.toString());
  };

  virtual QVariant valueToVariant() {
    return CALL_MEMBER_FN(targetClass, getter)();
  };

  virtual QVariant valueToString() {
    return CALL_MEMBER_FN(targetClass, getter)();
  };
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

  virtual void augment() { 
    target->append(T());
  };
  
};

/// This template class deals with the specific case of QList of
/// children of Serializable (but not pointers).
template <class T>
class SerializationQHash : public SerializationHash {
  QHash<QString, T> * target;
public:
  SerializationQHash(QHash<QString, T> * t, QString kn = "name") { 
    target = t;
    keyName = kn;
  };
  
  virtual QStringList keys() { return target->keys();};

  virtual Serializable * value(const QString &key) { 
    return &target->operator[](key);
  };

  virtual void insert(const QString & key, Serializable * el) {
    target->insert(key, *static_cast<T*>(el));
  };

  virtual void newElement(const QString & key) {
    target->insert(key, T());
  };
};

#endif
