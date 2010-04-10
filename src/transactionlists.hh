/** 
    \file transactionlists.hh
    Various Transaction lists
    Copyright 2010 by Vincent Fourmond

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


#ifndef __TRANSACTIONLISTS_HH
#define __TRANSACTIONLISTS_HH

#include <transaction.hh>

/// This class represents a list of Transaction objects, that can
/// potentially be modified, but not stored unlike TransactionList. It
/// is great for working on a sublist of a TransactionList.
///
/// It (will) provides quite a lot of functionality for obtaining
/// statistics.
class TransactionPtrList : public QList<Transaction*> {
public:
  
};

/// This class represents a list of Transaction objects, ready for
/// storage, with a few additional functionalities.
///
/// \todo Write a function that transforms this into a TransactionPtrList
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

  /// Removes the transactions in this list which are already present
  /// in the other list. Returns the number of duplicates removed.
  ///
  /// Both lists \b must be sorted !
  int removeDuplicates(const TransactionList & other);

  /// Makes sure the list only contains references to the given
  /// account, by removing all accounts that point to some different
  /// account, and setting NULL values to the given pointer.
  ///
  /// \todo There will probably be more cleanup to do around this.
  void sanitizeList(Account * ac);

  /// Transforms into a pointer list, for further manipulations.
  TransactionPtrList toPtrList();
};

#endif
