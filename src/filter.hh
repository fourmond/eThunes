/** 
    \file filter.hh
    The Filter class
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


#ifndef __FILTER_HH
#define __FILTER_HH

#include <serializable.hh>
#include <transaction.hh>

/// This is a simple filter element, that applies to a single
/// attribute of a target Transaction, and returns yes/no.
class FilterElement : public Serializable {
public:
  virtual SerializationAccessor * serializationAccessor();

  /// The target string in the Transaction to test
  enum {
    Name,
    Memo
  } transactionAttribute;
  
  /// The string to match it against.
  QString match;

  /// Regular expression ?
  bool regexp;

  /// Whether the element matches a target transaction or not.
  bool matches(Transaction * t);

  FilterElement() : transactionAttribute(Name), regexp(false) {;};
};

/// This class represents a simple filter for setting the categories.
///
/// \todo when tags are implemented, this should handle tags too.
class Filter : public Serializable {
public:

  /// Name of the filter
  QString name;

  /// Whether it is active or not
  bool active;

  /// List of the elements of the filter
  QList<FilterElement> elements;

  /// Should the Filter match all (false) or any (true) element ?
  bool matchAny;

  /// The target Category
  QString category;

  /// Implementation of the Serialization accessor
  virtual SerializationAccessor * serializationAccessor();

  Filter();

  /// Whether or not
  bool matches(Transaction * t);

  /// Loops over the list and sets the categories according to the tests.
  void processList(TransactionList * l);
};

#endif
