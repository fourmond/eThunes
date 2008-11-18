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
class Account {
};

/// Represents one transaction in a bank account.
/// Members are public for ease of use
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

  /// The account. NULL means that the transaction is not valid.
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

};

#endif
