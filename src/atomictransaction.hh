/**
   \file atomictransaction.hh
   The base for all Transaction-like object, including sub transactions
   Copyright 2008,2010,2012,2017 by Vincent Fourmond

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


#ifndef __ATOMICTRANSACTION_HH
#define __ATOMICTRANSACTION_HH

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

  /// A user-set comment
  QString comment;

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

  AtomicTransaction(int amount = 0, Transaction * bt = NULL);

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

  /// Sets the given named tag
  void setTagFromName(const QString & str, Wallet * wallet = NULL);

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

  /// Balance is not necessary meaningful
  virtual int getBalance() const {
    return 0;
  };


  /// Returns a user-set comment
  const QString & getComment() const {
    return comment;
  };

  /// Sets the comment  
  void setComment(const QString & cmt) {
    setAttribute(comment, cmt, "comment");
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

  /// Returns a string made either of the memo or the check number.
  QString getDescription() const {
    QString s = getMemo();
    if(! s.isEmpty()) 
      return s;
    s = getCheckNumber();
    if(! s.isEmpty())
      return QObject::tr("Check: %1").arg(s);
    return QString();
  };


public:

  virtual QString typeName() const {
    return "transaction";
  };

  virtual QString publicLinkName() const override;

  virtual void followLink();

private:

  /// Same as setTagList, but using the currently serialized wallet.
  void setTagListPrivate(const QString & str);

  /// Same as setCategoryFromName, but using the currently serialized
  /// wallet.
  void setCategoryFromNamePrivate(const QString & str);

};

#endif