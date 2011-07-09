/**
    \file transaction.hh
    Transaction, and transaction lists
    Copyright 2008,2010 by Vincent Fourmond

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


// Necessary classes
class Wallet;
class Account;

/// Represents one transaction in a bank account.
/// Members are public for ease of use.
///
/// \todo We are missing several attributes:
///
/// \li first, it would be interesting to add tags in addition to
///   categories, to be able to estimate the cost of something in a
///   way orthogonal to caterogies. Typical use: "Trip to Venice" ?
///
/// \li second, we need means to "subdivise" an atomic transaction into
///   several parts, because it often is the case that one bill falls
///   into several categories (this could be modified without
///   unlocking)
///
/// \li third, it would be interesting to save somewhere the "origin" of
///   the transaction: does it come straight from the bank or has it
///   been entered manually (or modified manually) ? This would allow
///   to "enter" transactions manually and then check them against web
///   download ?
class Transaction : public Linkable {
public:

  /// \name Global functions
  ///
  /// These functions are somewhat normative of how a transaction
  /// should look, and how to acquire various data and so on. This is
  /// of course a very precise definition
  ///
  /// @{

  /// A global function to format amount
  static inline QString formatAmount(int amount) {
    return QString("%1").arg(amount * 0.01, 0, 'f',2);
  };

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

  /// The amount of the transation, in cents
  int amount;

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
  /// Along with their accessors.
  /// 
  /// @{

  /// Whether the transaction is locked for manual modification or not.
  /// By default, they are all locked.
  bool locked;

  /// The main category. NULL means no category.
  Category * category;


  /// Whether the transaction has been added recently or not (when the
  /// user has already seen it). This flag should be set for every
  /// data import.
  bool recent;

  /// The list of tags.
  TagList tags;

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

  /// We make OFXImport a friend class that
  friend class OFXImport;

public:

  /// \name Accessors
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

  /// Returns the string representing the check number, or the empty
  /// string if the Transaction doesn't originate from a check.
  const QString & getCheckNumber() const {
    return checkNumber;
  };

  /// Returns the date
  const QDate & getDate() const {
    return date;
  };

  /// Returns the amount of the transaction.
  int getAmount() const {
    return amount;
  };

  /// Returns the amount of the transaction as a string.
  QString getAmountString() const {
    return formatAmount(amount);
  };

  /// Returns the name of the transaction.
  const QString &getName() const {
    return name;
  };

  /// Returns the memo. For display purposes, you'd rather want
  /// getDescription().
  const QString & getMemo() const {
    return memo;
  };

  /// Returns a string made either of the memo or
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
  Account * account;

  
  /// @}

public:

  Transaction();

  /// Dump debug output about the transaction to the given stream
  void dump(QIODevice *);

  void dump(QTextStream & stream);

  /// Returns the month ID for this transaction
  int monthID() const { return monthID(date); };

  /// Implements the comparison for sorting. Based on the date.
  ///
  /// \todo implement a full sort so that two slightly different items
  /// won't be equal
  bool operator<(const Transaction & t) const;

  bool operator==(const Transaction & t) const;

  virtual SerializationAccessor * serializationAccessor();

  /// Compares the checkNumber of two transactions (to use for qSort,
  /// for instance)
  static bool compareCheckNumbers(Transaction * a, Transaction * b);

  /// Fills in a AttributeHash with the Transaction
  /// information. Mostly useful for feeding the data to interpreted
  /// scripts.
  AttributeHash toHash() const;

  /// Returns a string that identifies uniquely a transaction within
  /// the account it's in.
  QString transactionID() const;

  // Linkable functions

  virtual QString uniqueID() const;

  virtual QString typeName() const {
    return "transaction";
  };

  virtual QString publicTypeName() const {
    return QObject::tr("Transaction");
  };

  virtual void followLink();


private:

  /// Same as before, but using the currently serialized wallet.
  void setTagListPrivate(const QString & str);
  

};

#endif
