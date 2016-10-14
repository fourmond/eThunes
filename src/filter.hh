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
#include <transactionlists.hh>

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
  bool matches(const Transaction * t) const;

  /// Needed to clear regexp
  virtual void prepareSerializationRead();

  FilterElement() : transactionAttribute(Name), regexp(false) {;};
};

/// This class represents the action taken on a Transaction that is
/// matched by a Filter.
class FilterAction : public Serializable {
public:
  /// The type of action
  enum {
    AddTag,                     // Adds the given tag
                                // (unless already present)
    SetCategory                 // sets the category (unless a
                                // category is present already)
  } actionType;

  /// An auxiliary string used with a meaning dependent on the action.
  QString what;

  virtual SerializationAccessor * serializationAccessor();

  /// Performs the action
  void performAction(Transaction * target) const;

  FilterAction(const QString & cat = QString());

};

/// This class represents a simple filter for setting the categories.
///
/// \todo when tags are implemented, this should handle tags too.
class Filter : public Serializable {

  /// (deprecated) the target Category
  QString category;

protected:

  virtual void finishedSerializationRead() override;

public:

  /// Name of the filter
  QString name;

  /// Whether it is active or not
  bool active;

  /// List of the elements of the filter
  QList<FilterElement> elements;

  /// List of the actions to be performed by the filter
  QList<FilterAction> actions;

  /// Should the Filter match all (false) or any (true) element ?
  bool matchAny;


  /// Implementation of the Serialization accessor
  virtual SerializationAccessor * serializationAccessor();

  Filter();

  /// Whether or not target this Filter matches the given Transaction
  bool matches(const Transaction * t) const;

  /// Loops over the list and sets the categories according to the tests.
  void processList(TransactionList * l) const;

  /// Returns the matching transactions of the given list.
  TransactionPtrList matchingTransactions(TransactionList * l) const;

  /// Performs all the actions in the list on t
  void performActions(Transaction * t) const;
};

#endif
