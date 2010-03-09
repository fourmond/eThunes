/** 
    \file account.hh
    The Account class
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


#ifndef __ACCOUNT_HH
#define __ACCOUNT_HH

#include <transaction.hh>

/// Represents informations about an account.
///
/// \todo this class and all the other serializable ones should be the
/// child of one XMLSerializable class that could handle simple
/// attributes through hashes (fromHash, toHash method to be
/// reimplemented by children) + more complex structure such as lists
/// of other XML elements...
///
/// \todo Maybe this class and all the other ones should join a Money
/// namespace of some kind when the program sees the birth of the new
/// functionalities ?
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
  QString accountID() const {
    return bankID + "-" + branchID + "-" + accountNumber;
  };

  /// @}

  /// The transactions of the account, ordered by date (0 = most
  /// ancient transaction), so that the computation of the balance is
  /// possible
  QList<Transaction> transactions;

  /// Import the given list of transactions into the current
  /// account. It does reorder the transaction list so that they are
  /// sorted by date.
  int importTransactions(QList<Transaction> transactions);

  /// The user-given name for the account.
  QString publicName;

  /// Returns a name for the account, based on publicName if available
  /// or on accountID if that isn't the case.
  QString name();

  /// Returns true if the given Account object is the same account
  /// as this one.
  int isSameAccount(const Account & ac) const { 
    return ac.accountID() == accountID();
  };
};

#endif
