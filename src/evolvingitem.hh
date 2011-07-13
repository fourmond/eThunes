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


  /// A list of date, item pairs recording the changes in the value of
  /// the item. Whatever its value, the first date \b will be ignored.
  QList<StoragePair> itemChanges; 


  EvolvingItem(const T & t) { 
    itemChanges.append(StoragePair(QDate(), t));
  };

  /// Returns the value at the given time
  T operator[](const QDate & when) const {
    T ret = itemChanges[0].value;
    for(int i = 1; i < itemChanges.size(); i++)
      if(when >= itemChanges[i].date)
        ret = itemChanges[i].value();
    return ret;
  };

  SerializationAccessor * serializationAccessor() {
    SerializationAccessor * ac = new SerializationAccessor(this);
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
};

#endif
