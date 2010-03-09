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
///
class Transaction {
public:

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

  /// The check number, if applicable. 0 means no number.
  int checkNumber;

  /// The account of the transaction. NULL means no account, ie the
  /// transaction is invalid.
  Account * account;

  /// @}

  /// \name User-defined attributes
  ///
  /// @{

  /// Whether the transaction is locked for manual modification or not.
  /// By default, they are all locked.
  int locked;

  /// The main category. Empty means no category.
  ///
  /// \todo Provide a way to automatically select the category based
  /// on regular expressions, or other kinds of "filters" (a bit like
  /// the mail filters ?)
  QString category;

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

  Transaction();

  /// Dump debug output about the transaction to the given stream
  void dump(QIODevice *);

  void dump(QTextStream & stream);

  /// Implements the comparison for sorting. Based on the date.
  bool operator<(const Transaction & t) const { return date < t.date;};

};

/// \todo We should provide a function that can easily check for
/// duplicates. Tough one, though, if it is possible that twice the
/// same transaction happens on the same day. It should be possible to
/// do that across two TransactionList, where one would have potential
/// duplicates marked using a boolean value of some kind.
class TransactionList : public QList<Transaction> {
public:

  TransactionList() {;};
  TransactionList(const  QList<Transaction> & l) : QList<Transaction>(l) {;};

  /// Sorts the list according to the transaction date.
  void sortByDate();

  /// Returns a sublist in the same order as this one containing only
  /// transactions dealing with the given account (as per
  /// Account.isSameAccount()).
  TransactionList sublist(const Account & account);

  /// Compute the balance for each element of the list. Does not sort
  /// the list beforehand. You'll have to do it yourself.
  void computeBalance(int initialBalance = 0);
};

#endif
