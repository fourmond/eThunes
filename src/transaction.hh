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

#include <serializable.hh>
#include <category.hh>
#include <tag.hh>
#include <attributehash.hh>
#include <linkable.hh>
#include <watchablecontainers.hh>

// Necessary classes
class Wallet;
class Account;

class Transaction;

/// This reprensents an atomic transaction, ie what is left after you
/// split a Transaction into several sub-transactions. 
class AtomicTransaction : public Linkable {
protected:
  /// The amount of the transation, in cents
  int amount;

  /// The category
  Category * category;

  /// The list of tags.
  TagList tags;

public:

  /// The transaction this one is derived from. If not NULL, then this
  /// is a sub-transaction.
  Transaction * baseTransaction;


  /// Returns a unique monthID for the given month
  static inline int monthID(const QDate &date) {
    return date.month()-1 + date.year() * 12;
  };

  /// Returns a date from a monthID
  static inline QDate dateFromID(int monthID, int day = 1) {
    return QDate(monthID/12, monthID % 12 + 1, day);
  };

  /// Returns the monthID for this month
  static inline int thisMonthID() {
    return monthID(QDate::currentDate());
  };

  /// Returns the monthID for last month
  static inline int lastMonthID() {
    return monthID(QDate::currentDate()) - 1; /// \todo hard-wired to
					      /// the representation
					      /// of monthID
  };

  AtomicTransaction();

  /// Returns the Account associated to the AtomicTransaction, or NULL
  /// if there isn't.
  virtual Account * getAccount() const;

  /// \name Accessors
  ///
  /// Returns the various attributes linked to the
  /// AtomicTransaction. Most of them are derived from the
  /// baseTransaction.
  ///
  /// @{


  /// Returns the list of tags, formatted as in TagList::toString.
  QString tagString() const {
    return tags.toString();
  };

  /// Sets the tag list, from a comma-separated string. The wallet is
  /// necessary.
  void setTagList(const QString & str, Wallet * wallet = NULL);

  /// Clears the given tag
  void clearTag(Tag * t) {
    tags.clearTag(t);
  }; 

  /// Sets the given tag
  void setTag(Tag * t) {
    tags.setTag(t);
  }; 

  /// Whether the taglist contains the the given tag
  bool hasTag(const Tag * t) const {
    return tags.hasTag(const_cast<Tag*>(t));
  }; 

  /// Sets the category from the given String. If wallet is NULL, it
  /// is taken to be account->wallet, which shouldn't be NULL.
  void setCategoryFromName(const QString & str, Wallet * wallet = NULL);

  /// Returns the full name of the Category, suitable for saving, or
  /// an empty string when there isn't a category.
  QString categoryName() const ;

  /// Returns the category.
  Category * getCategory() const {
    return category;
  };

  /// sets the category.
  void setCategory(Category * c) {
    setAttribute(category, c, "category");
  };

  /// Returns the amount of the transaction. To be more precise, it is
  /// the amount that matches the Category and the Tag. For splitted
  /// transactions, this will only correspond to the difference
  /// between the subtransactions and this one.
  virtual int getAmount() const {
    return amount;
  };

  virtual int getTotalAmount() const {
    return amount;
  };

  virtual QString getCheckNumber() const;

  /// Returns the date
  virtual QDate getDate() const;

  /// Returns the month ID for this transaction
  int monthID() const;

  /// Returns the name of the transaction.
  virtual QString getName() const;

  /// Returns the memo.
  virtual QString getMemo() const;
  /// @}

  virtual SerializationAccessor * serializationAccessor();

  /// Fills in a AttributeHash with the Transaction
  /// information. Mostly useful for feeding the data to interpreted
  /// scripts.
  AttributeHash toHash() const;

public:

  virtual QString typeName() const {
    return "transaction";
  };

  virtual QString publicTypeName() const {
    return QObject::tr("Transaction");
  };

  virtual void followLink();

private:

  /// Same as setTagList, but using the currently serialized wallet.
  void setTagListPrivate(const QString & str);

  /// Same as setCategoryFromName, but using the currently serialized
  /// wallet.
  void setCategoryFromNamePrivate(const QString & str);

};

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
  int getBalance() const {
    return balance;
  };

  /// Gets the balance as a string
  QString getBalanceString() const {
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

  /// Returns the amount of the transaction as a string.
  QString getAmountString() const {
    return formatAmount(amount);
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

  /// Returns a string made either of the memo or the check number.
  QString getDescription() const {
    if(! memo.isEmpty()) 
      return memo;
    if(! checkNumber.isEmpty())
      return QObject::tr("Check: %1").arg(checkNumber);
    return QString();
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
