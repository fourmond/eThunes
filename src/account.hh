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

#include <serializable.hh>
#include <transaction.hh>
#include <transactionlists.hh>
#include <filter.hh>

/// Represents informations about an account.
///
/// \todo Maybe this class and all the other ones should join a Money
/// namespace of some kind when the program sees the birth of the new
/// functionalities ?
class Account : public Serializable {
public:

  /// \name Bank-given attributes
  ///
  /// These attributes are found in raw OFX files or other exports from
  /// the bank.
  ///
  /// @{
  ///
  /// @todo These attributes will have to be signalled one day, but as
  /// they hardly ever change, I don't really see the point.

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

  Account();

  /// The Wallet to which this account belongs to.
  Wallet * wallet;

  /// The transactions of the account, ordered by date (0 = most
  /// ancient transaction), so that the computation of the balance is
  /// possible
  TransactionList transactions;

  /// Returns the monthID of the earliest month of the account, or -1
  /// if there are not transactions. (but that shouldn't happen,
  /// shouldn't it ?)
  int firstMonthID() const;

  /// Import the given list of transactions into the current
  /// account. It does reorder the transaction list so that they are
  /// sorted by date.
  ///
  /// If the pointer to the Wallet NULL the filters are run
  /// on the new transactions.
  int importTransactions(TransactionList transactions,
			 Wallet * walletFilters = NULL);

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

  /// Returns the current balance, in cents
  int balance() const { if(transactions.size() > 0)
      return transactions.last().getBalance(); return 0;
  };

  /// Implementation of the Serialization accessor
  virtual SerializationAccessor * serializationAccessor();

  /// Sanitizes the account, by ensuring the transactions belong to
  /// this account and computing the balance
  ///
  /// \todo Maybe this should be a slot, one day...
  void sanitizeAccount();

  /// Clears the contents of the account.
  void clearContents();

  // Serialize reimplementation
  virtual void prepareSerializationRead() { clearContents();};
  virtual void finishedSerializationRead() { sanitizeAccount();};


  /// Returns the Transaction objects of the account that belong to
  /// the given Category, or possibly to one of its descendants.
  ///
  /// @todo make that a TransactionPtrList one day ?
  QList<Transaction *> categoryTransactions(const Category * category,
					    bool parents = true);


  /// Returns a list of checks (for which checkNumber is not empty).
  TransactionPtrList checks();

  /// Returns the list of recent transactions.
  TransactionPtrList recentTransactions();

  /// Returns the list of the transactions of the given month.
  TransactionPtrList monthlyTransactions(int monthID);

  /// Returns the Transaction whose Transaction::transactionID()
  /// matches name.
  ///
  /// @deprecated
  Transaction * namedTransaction(const QString & name);
};

#endif
