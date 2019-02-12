/**
    \file wallet.hh
    Class representing a collection of accounts.
    Copyright 2010, 2012 by Vincent Fourmond

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


#ifndef __WALLET_HH
#define __WALLET_HH

#include <serializable.hh>
#include <category.hh>
#include <account.hh>
#include <ofximport.hh>
#include <filter.hh>
#include <accountgroup.hh>
#include <tag.hh>
#include <watchablecontainers.hh>
#include <budget.hh>

class Budget;
class Period;

/// This class represents a collection of accounts. Presumably, there
/// should be only one Wallet instance in a running program, although
/// I probably shouldn't rely on this assumption.
class Wallet : public Serializable {
public:

  Wallet();

  /// The accounts held within the wallet.
  WatchableList<Account> accounts;

  /// The month id of the first month
  int firstMonthID() const;

  /// The top-level categories of the wallet
  /// 
  /// @todo This and tags have to be made watchable
  CategoryHash categories;

  /// The tags
  TagHash tags;

  /// A list of Filter objects to run over the imported transactions.
  WatchableList<Filter> filters;

  /// A list of AccountGroup
  WatchableList<AccountGroup> accountGroups;

  /// The list of Budget objects
  WatchableList<Budget> budgets;

  /// Returns the list of all potential Link targets.
  QList<Linkable *> allTargets() const;


  /// Runs the filters on the given transaction list
  void runFilters(TransactionList * list);

  /// Runs the filters on all the account transactions
  void runFilters();

  /// \name Category- and tag-related functions
  ///
  /// @{

  /// Returns the Category object corresponding to the given name, or
  /// NULL if there isn't.
  Category * namedCategory(const QString &name);

  /// Returns the Tag object corresponding to the given name, or NULL
  /// if there isn't.
  Tag * namedTag(const QString &name);

  /// The list of Transaction objects that belong to one category. If
  /// \p parents is true (default), then we also look for children of
  /// the category.
  TransactionPtrList categoryTransactions(const Category * category,
					  bool parents = true);

  /// The list of Transaction objects that belong to one category. If
  /// \p parents is true (default), then we also look for category in
  /// the transactions ancestry.
  TransactionPtrList taggedTransactions(const Tag * tag);

  /// Returns all the transactions
  TransactionPtrList allTransactions() const;

  /// @}

  /// Import all the contents of an OFX import or something similar
  /// into the wallet. That means import the accounts and the
  /// transactions if necessary.
  void importAccountData(const OFXImport & data, bool runFilters = true);

  virtual SerializationAccessor * serializationAccessor();

  /// Clears the contents of the Wallet (such as before loading ;-)...
  void clearContents();

  /// This holds the wallet currently being loaded through
  /// serialization.
  ///
  /// \todo This will prevent parallel serialization; this could be
  /// changed into some class, to be seen later on.
  static Wallet * walletCurrentlyRead;

  virtual void prepareSerializationRead();

  virtual void finishedSerializationRead();

  /// Returns the Account whose Account::accountID() matches name.
  Account * namedAccount(const QString & name);

  /// Returns the Transaction whose Transaction::uniqueID() matches name.
  Transaction * namedTransaction(const QString & name);

  /// Returns all the Transaction that match the given filter.
  TransactionPtrList transactionsForFilter(const Filter * filter);

  /// Attemps to find internal moves, using
  /// TransactionPtrList::findInternalMoves()
  void findInternalMoves(bool permissive = false);


public:

  /// Returns all the transactions within the given date range.
  TransactionPtrList transactionsForPeriod(const Period & period);

  /// Returns the overall balance for all the accounts
  int balance(const QDate & date) const;

};

#endif
