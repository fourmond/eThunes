/**
    \file pointersafesort.hh: 
    qSort on value lists keeping the pointers meaningful
    Copyright 2012 by Vincent Fourmond

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

/// Here begins the code for pointer-safe sorting:
template <typename T> class MyQListIterator {
public:
  QList<T> * list;

  int idx;

  /// The exact same thing as QListIterator, but under another name
  /// and with different semantics.
  class PointedTo {
  public:
    QList<T> * list;

    int idx;

    const T & value() const {
      return (*list)[idx];
    };

    PointedTo(const MyQListIterator & i) : list(i.list), idx(i.idx) {;};
    
    bool operator <(const PointedTo & b) const {
      return value() < b.value();
    };

  };
  
  MyQListIterator(QList<T> * l, int i) : list(l), idx(i) {;};

  PointedTo operator*() { return PointedTo(*this);};

  MyQListIterator operator+(int i) const { 
    return MyQListIterator(list, idx+i);
  };

  MyQListIterator operator-(int i) const { 
    return MyQListIterator(list, idx-i);
  };

  bool operator<(const MyQListIterator & b) const {
    return idx < b.idx;
  };

  bool operator!=(const MyQListIterator & b) const {
    return idx != b.idx;
  };

  MyQListIterator& operator++() {
    idx++;
    return *this;
  };

  MyQListIterator& operator--() {
    idx--;
    return *this;
  };

  operator int() const {
    return idx;
  };

  static void swap(PointedTo a, PointedTo b) 
  {
    T * origa = &((*a.list)[a.idx]);
    a.list->swap(a.idx, b.idx);
    // We check that the pointers got swapped correctly (at least one of
    // them)
    T * finala = &((*a.list)[b.idx]);
    Q_ASSERT(origa == finala);
  }
  
};

// Funny, this should be explicit and not a template, else g++ is
// unable to pick it.
template <typename T> void qSwap(typename MyQListIterator<T>::PointedTo a, 
                                 typename MyQListIterator<T>::PointedTo b) 
{
  MyQListIterator<T>::swap(a,b);
}

template <typename T>
void pointerSafeSortList(QList<T> * list)
{
  qSort(MyQListIterator<T>(list, 0),
        MyQListIterator<T>(list, list->size()));
}

