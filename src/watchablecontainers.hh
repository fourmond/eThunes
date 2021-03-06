/**
    \file watchablecontainers.hh
    Templates for container classes with signalling built-in
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


#ifndef __WATCHABLECONTAINERS_HH
#define __WATCHABLECONTAINERS_HH

#include <watchable.hh>

/// This template class wraps around a QList to provide signalling for
/// its various operations. This list does not attempt to watch
/// children.
template <class T> class WatchedList : public Watchable {
private:
  /// The internal container.
  QList<T> data;
protected:
  
  /// Gets a reference to a value, bypassing signalling
  T & unwatchedValue(int i) {
    return data[i];
  };

  /// An access to raw data
  const QList<T> & constData() const {
    return data;
  };

  
public:

  WatchedList() {;};
  WatchedList(const QList<T> & d) : data(d) {;};

  /// An access to raw data
  QList<T> & rawData() {
    attributeChanged("members");
    return data;
  };

  /// \name Accessors to the targets.
  ///
  /// @{

  const T & operator[](int i) const {
    return data[i];
  };

  T & operator[](int i) {
    // Signalling
    attributeChanged("members");
    return unwatchedValue(i);
  };

  const T & value(int i) const {
    return (*this)[i];
  };

  T value(int i, const T & v) const {
    return data.value(i, v);
  };

  const T & first() const {
    return data.first();
  };


  const T & last() const {
    return data.last();
  };

  T & last() {
    return data.last();
  };

  const T & at(int i ) const {
    return data.at(i);
  };


  virtual void removeAt(int i) {
    data.removeAt(i);
    numberChanged();
    objectRemoved(i, 1);
  };

  virtual T takeAt(int i) {
    T rv = at(i);
    removeAt(i);
    return rv;
  }

  typename QList<T>::iterator begin() {
    return data.begin();
  };

  typename QList<T>::const_iterator begin() const {
    return data.begin();
  };

  typename QList<T>::iterator end() {
    return data.end();
  };
  
  typename QList<T>::const_iterator end() const {
    return data.end();
  };

  /// Appends. We make it a virtual function
  virtual void append(const T& d) {
    data.append(d);
    numberChanged();
    objectInserted(data.size() - 1, 1);
  };

  void replace(int i, const T & v) {
    attributeChanged("members");
    data.replace(i, v);
  };

  WatchedList<T> & operator<<(const T& a) {
    append(a);
    return *this;
  };

  /// Append list
  void append(const WatchedList& list) {
    int at = data.size();
    data.append(list);
    numberChanged();
    objectInserted(data.size(), list.size());
  };

  int size() const {
    return data.size();
  };

  int count() const {
    return data.count();
  };

  void clear() {
    int sz = data.size();
    data.clear();
    numberChanged();
    objectRemoved(0, sz);
  };

  /// @}
  
};

/// This is the same as WatchedList, excepted that the target type is
/// a Watchable child, and we watch for it.
template <class T> class WatchableList : public WatchedList<T> {
public:
  WatchableList() {;};
  WatchableList(const QList<T> & d) : WatchedList<T>(d) {
    watchAll();
  };

  T & operator[](int i) {
    /// The target is Watchable, it'll handle itself the changes.
    return this->unwatchedValue(i);
  };

  const T & operator[](int i) const {
    return WatchedList<T>::operator[](i);
  };

  virtual void append(const T& d) {
    WatchedList<T>::append(d);
    this->watchChild(&this->unwatchedValue(WatchedList<T>::size()-1), "members");
  };

  void append(const WatchableList& list) {
    int size = list.size();
    for(int i = 0; i < size; i++)
      append(list[i]);
  };

  /// Ensures all children are watched for.
  void watchAll() {
    int size = WatchedList<T>::size();
    for(int i = 0; i < size; i++)
      this->watchChild(&this->unwatchedValue(i), "members");
  };

  QList<T *> pointerList() {
    QList<T *> rv;
    for(int i = 0; i < this->size(); i++)
      rv << &((*this)[i]);
    return rv;
  };

};


/// This is the same as WatchedList, excepted that the target type is
/// a Watchable child, and we watch for it.
template <class T> class WatchablePtrList : public WatchedList<T*> {
  typedef T* pointer;
public:
  WatchablePtrList() {;};
  WatchablePtrList(const QList<T*> & d) : WatchedList<T*>(d) {
    watchAll();
  };

  pointer & operator[](int i) {
    /// The target is Watchable, it'll handle itself the changes.
    return WatchedList<T*>::unwatchedValue(i);
  };

  const pointer &  operator[](int i) const {
    return WatchedList<T*>::operator[](i);
  };

  virtual void append(T * d) {
    WatchedList<T*>::append(d);
    this->watchChild(this->unwatchedValue(WatchedList<T*>::size()-1), "members");
  };

  void append(const WatchablePtrList& list) {
    int size = list.size();
    for(int i = 0; i < size; i++)
      append(list[i]);
  };

  /// Ensures all children are watched for.
  void watchAll() {
    int size = WatchedList<T*>::size();
    for(int i = 0; i < size; i++)
      this->watchChild(WatchedList<T*>::unwatchedValue(i), "members");
  };

};



#endif
