/**
   \file evolvingitem.hh
   Object whose value depends on time.
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

#ifndef __EVOLVINGITEM_HH
#define __EVOLVINGITEM_HH

#include <serializable.hh>
#include <serializable-templates.hh>


/// This template class provides storage for a item whose value may
/// depend on time, in discrete steps.
template <class T> class EvolvingItem : public Serializable {
public:

  /// A class to store a date/value pair.
  class StoragePair: public Serializable {
  public:
    QDate date;
    T value;

    virtual SerializationAccessor * serializationAccessor() {
      SerializationAccessor * ac = new SerializationAccessor(this);
      ac->addScalarAttribute("date", &date);
      ac->addScalarAttribute("value", &value);
      return ac;
    };

    /// A comparison operator for sorting.
    bool operator<(const StoragePair & a) const {
      return date < a.date;
    };

    StoragePair(const QDate & d, const T & t) : 
      date(d), value(t) {;};

    StoragePair() {;};
  };

  /// The first value.
  T firstValue;
  
  /// A list of date, item pairs recording the changes in the value of
  /// the item.
  QList<StoragePair> itemChanges; 


  EvolvingItem(const T & t) : firstValue(t) { 
  };

  /// Returns the value at the given time
  T operator[](const QDate & when) const {
    T ret = firstValue;
    for(int i = 0; i < itemChanges.size(); i++)
      if(when >= itemChanges[i].date)
        ret = itemChanges[i].value;
    return ret;
  };

  /// Returns a pointer to the numbered value. 0 is firstValue
  T & valueAt(int i) {
    if(i > 0)
      return itemChanges[i-1].value;
    else
      return firstValue;
  };

  QDate & dateAt(int i) {
    if(i > 0)
      return itemChanges[i-1].date;
    else
      return *static_cast<QDate*>(NULL);
  };

  StoragePair pairValue(int i) {
    if(i > 0)
      return StoragePair(dateAt(i), valueAt(i));
    return StoragePair(QDate(), firstValue);
  };

  SerializationAccessor * serializationAccessor() {
    SerializationAccessor * ac = new SerializationAccessor(this);
    ac->addScalarAttribute("first-value", &firstValue);
    ac->addListAttribute("item-change", &itemChanges);
    return ac;
  };

  void sort() {
    qSort(itemChanges);
  };

  /// Add a change date
  void addChange(const QDate & date, const T & value) {
    itemChanges.append(StoragePair(date, value));
    sort();
  };

  /// Remove the given change
  void removeChange(int i) {
    if(i)
      itemChanges.removeAt(i);
  };

  /// Returns a String representation of the value.
  QString toString() const {
    QString retval;
    retval = QString("%1").arg(firstValue);
    for(int i = 0; i < itemChanges.size(); i++)
      retval += QObject::tr(" until %1, then %2").
        arg(itemChanges[i].date.toString()).
        arg(itemChanges[i].value);
    return retval;
  };

  QString toString(QString (*formatter)(T)) const {
    QString retval;
    retval = QString("%1").arg(formatter(firstValue));
    for(int i = 0; i < itemChanges.size(); i++)
      retval += QObject::tr(" until %1, then %2").
        arg(itemChanges[i].date.toString()).
        arg(formatter(itemChanges[i].value));
    return retval;
  };

  /// Convenience conversion operator, to be dropped whenever
  /// possible.
  ///
  /// @deprecated.
  operator T () const __attribute__((deprecated)) {
    return firstValue;
  }
};

#endif
