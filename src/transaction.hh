/**
   \file transaction.hh
   Transaction and sub transactions
   Copyright 2008,2010,2012 by Vincent Fourmond

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


#ifndef __TRANSACTION_HH
#define __TRANSACTION_HH

#include <atomictransaction.hh>
#include <serializable.hh>
#include <category.hh>
#include <tag.hh>
#include <attributehash.hh>
#include <linkable.hh>
#include <watchablecontainers.hh>


/// Represents one transaction in a bank account.
///
/// \todo it would be interesting to save somewhere the "origin" of
/// the transaction: does it come straight from the bank or has it
/// been entered manually (or modified manually) ? This would allow to
/// "enter" transactions manually and then check them against web
/// download ?
class Transaction : public AtomicTransaction {
public:

  /// \name Global functions
  ///
  /// These functions are somewhat normative of how a transaction
  /// should look, and how to acquire various data and so on. This is
  /// of course a very precise definition
  ///
  /// @{

  /// If this global parameter is on, the amounts displayed will be
  /// displayed modulo that number.
  static int formatAmountModulo;

  /// A global function to format amount
  static inline QString formatAmount(int amount) {
    if(formatAmountModulo)
      amount %= formatAmountModulo;
    return QString("%1").arg(amount * 0.01, 0, 'f',2);
  };



  /// @}

protected:

  /// \name Bank-given attributes
  ///
  /// These attributes are found in raw OFX files or other exports from
  /// the bank.
  ///
  /// These are locked by default: the user cannot modify them unless
  /// he explicitly unlocks them.
  ///
  /// @{

  /// The name of the transaction
  QString name;

  /// A small text associated with the transaction
  QString memo;

  /// The date of the transaction
  QDate date;

  /// The check number, if applicable
  QString checkNumber;

  /// @}

  /// \name User-defined attributes
  ///
  /// @{

  /// Whether the transaction is locked for manual modification or not.
  /// By default, they are all locked.
  ///
  /// @todo This currently isn't used.
  bool locked;

  /// Whether the transaction has been added recently or not (when the
  /// user has already seen it). This flag should be set for every
  /// data import.
  bool recent;

  /// @}

  /// \name Internal attributes
  ///
  /// @{

  /// The balance of the corresponding account after the transaction
  /// has been performed.
  int balance;

  /// balance only has a meaning when this parameter isn't false.
  bool balanceMeaningful;

  /// @}

  /// We make OFXImport a friend class.
  friend class OFXImport;

public:

  /// The list of all the sub transactions but the one derived from
  /// this one (ie, to complement)
  ///
  /// @todo Do we need this to be protected ?
  WatchableList<AtomicTransaction> subTransactions;


  /// \name Accessors
  /// @{ 

  virtual int getAmount() const;

  /// Sets the "recent" status of the Transaction.
  void setRecent(bool rec = true) {
    setAttribute(recent, rec, "recent");
  };

  /// Whether the Transaction is marked as recent or not.
  bool isRecent() const {
    return recent;
  };

  /// Sets the balance
  void setBalance(int b) {
    setAttribute(balance, b, "balance");
  };

  /// Gets the balance
  virtual int getBalance() const {
    return balance;
  };

  /// Gets the balance as a string
  QString getBalanceString() const __attribute__((deprecated)) {
    return formatAmount(balance);
  };


  /// Returns the string representing the check number, or the empty
  /// string if the Transaction doesn't originate from a check.
  virtual QString getCheckNumber() const {
    return checkNumber;
  };

  /// Returns the date
  virtual QDate getDate() const {
    return date;
  };

  /// Returns the name of the transaction.
  virtual QString getName() const {
    return name;
  };

  /// Returns the memo. For display purposes, you'd rather want
  /// getDescription().
  virtual QString getMemo() const {
    return memo;
  };


  /// @}

public:

  /// \name Public attributes
  ///
  /// @{

  /// The account of the transaction. NULL means no account, ie the
  /// transaction is invalid.
  ///
  /// @todo This probably should be turned protected with appropriate
  /// accessors.
  Account * account;

  virtual Account * getAccount() const;
  
  /// @}

public:

  Transaction();

  /// Creates a dummy transaction at the given date with the given
  /// balance.
  Transaction(const QDate & date, int balance);

  /// Dump debug output about the transaction to the given stream
  void dump(QIODevice *);

  void dump(QTextStream & stream);

  /// Implements the comparison for sorting. Based on the date.
  ///
  /// \todo implement a full sort so that two slightly different items
  /// won't be equal
  bool operator<(const Transaction & t) const;

  bool operator==(const Transaction & t) const;

  virtual SerializationAccessor * serializationAccessor();
  virtual void prepareSerializationRead();
  virtual void finishedSerializationRead();

  /// Compares the checkNumber of two transactions (to use for qSort,
  /// for instance)
  // static bool compareCheckNumbers(Transaction * a, Transaction * b);
  static bool compareCheckNumbers(AtomicTransaction * a, AtomicTransaction * b);

  /// Returns a string that identifies uniquely a transaction within
  /// the account it's in.
  ///
  /// @todo I'm unsure this is still used. Probably should be removed ?
  QString transactionID() const;

  /// Returns all the sub transactions (ie one more than there are
  /// elements in subTransactions)
  ///
  /// @todo I should write a proper iterator to browse through all the
  /// transactions.
  QList<AtomicTransaction*> allSubTransactions();


  /// @name Iterators
  ///
  /// Iterators for the transactions
  ///
  /// @{

  class iterator {
    friend class Transaction;
    Transaction * transaction;
    int idx;
    iterator(Transaction * trs, int i = -1) : 
      transaction(trs), idx(i) {
    };
  public:
    inline AtomicTransaction * operator*() { 
      if(idx >= 0)
        return &transaction->subTransactions[idx];
      else
        return transaction;
    };
    inline bool operator==(const iterator &o) const { 
      return (transaction == o.transaction) && (idx == o.idx);
    };
    inline bool operator!=(const iterator &o) const { return ! (*this == o); };
    inline iterator &operator++() { ++idx; return *this; };
  };

  iterator begin() { return iterator(this);};
  iterator end() { return iterator(this, subTransactions.size());};
  /// @}

};

#endif
