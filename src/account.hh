/** 
    \file account.hh
    Collection of objects that represent an account.
    Copyright 2008 by Vincent Fourmond

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


#ifndef __ACCOUNT_HH
#define __ACCOUNT_HH

/// Represents informations about an account. Does not in itself
/// contain transactional data.
///
/// This class corresponds more-or-less to the header in an OFX
/// file.
class Account {
public:

  /// \name Bank-given attributes
  ///
  /// These attributes are found in raw OFX files or other exports from
  /// the bank.
  /// 
  /// @{

  /// The different kinds of accounts (not much here, admittedly...)
  enum  AccountType { 
    Savings 
  };

  /// The type of the account.
  AccountType type;

  /// The bank account number.
  QString accountNumber;

  /// The Bank ID
  QString bankID;

  /// The branch ID
  QString branchID;

  /// A unique account identification.
  QString accountID() {
    return bankID + "-" + branchID + "-" + accountNumber;
  };

  /// @}

  /// The user-given name for the account.
  QString publicName;

  /// Returns a name for the account, based on publicName if available
  /// or on accountID if that isn't the case.
  QString name();

  /// Returns true if the given Account object is the same account
  /// as this one.
  int isSameAccount(const Account & ac) const;
};

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
  QString category;

  /// @}

  Transaction();

  /// Dump debug output about the transaction to the given stream
  void dump(QIODevice *);

  void dump(QTextStream & stream);

};

#endif
