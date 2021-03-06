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
#include <watchablecontainers.hh>

/// Statistics inherent to a series of transactions, from whichever
/// way they are organized.
class BasicStatistics {
public:

  BasicStatistics();

  /// Number of transactions
  int number;

  /// Number of credit transactions
  int numberCredit;

  /// Number of debit transactions
  int numberDebit;

  /// Total amount of the transactions
  int totalAmount;

  /// total amount of the credits
  int totalCredit;

  /// total amount of the debits
  int totalDebit;

  /// The first month ID of the accounts corresponding to the
  /// transactions.
  int firstMonthID;

  /// Adds the given Transaction to the statistics
  void addTransaction(const AtomicTransaction * t);

  /// Adds other stats to this one.
  BasicStatistics & operator+=(const BasicStatistics & stats);

};

/// A series of (detailed) statistics about a list of Transaction
/// objects; see TransactionPtrList.
class TransactionListStatistics : public BasicStatistics {
public:
  TransactionListStatistics();


  /// A hash of Transaction::monthID -> BasicStatistics
  QHash<int, BasicStatistics> monthlyStats;

  /// A hash of year -> BasicStatistics
  QHash<int, BasicStatistics> yearlyStats;

  /// Adds the given Transaction to the statistics
  void addTransaction(const AtomicTransaction * t);

  /// Returns the BasicStatistics for this month
  inline const BasicStatistics & thisMonthStats() {
    return monthlyStats[Transaction::thisMonthID()];
  };

  /// Returns the BasicStatistics for last month
  inline const BasicStatistics & lastMonthStats() {
    return monthlyStats[Transaction::lastMonthID()];
  };

  /// Returns the average amount for "complete months !" (ie before
  /// thisMonthID)
  int monthlyAverageAmount();
};

class Period;

/// This class represents a list of Transaction objects, that can
/// potentially be modified, but not stored unlike TransactionList. It
/// is great for working on a sublist of a TransactionList.
///
/// It (will) provides quite a lot of functionality for obtaining
/// statistics.
///
/// @todo This should be pointers to AtomicTransaction !
class TransactionPtrList : public WatchablePtrList<AtomicTransaction> {
public:

  /// Returns various interesting statistics about the list.
  TransactionListStatistics statistics() const;

  /// The type for a function to work on TransactionPtrList
  typedef std::function<void (const TransactionPtrList &)> Action;

  /// Browses through the TransactionList given here trying to find
  /// transaction that could correspond to both ends of the same
  /// operation, characterized by
  /// 
  /// \li same date
  /// \li different account
  /// \li opposed amounts
  /// \li same memo
  ///
  /// This may give false positives, but it's the best we can do.  You
  /// can optionnaly turn on the permissive flag which disables the
  /// check of the memo.
  ///
  /// @warning This function assumes that the transactions are sorted
  /// !
  ///
  /// It creates links between transactions found and returns the list
  /// of newly-created links.
  static QList<Link *> findInternalMoves(QList<TransactionPtrList> lists, 
                                         bool permissive = false);

  /// Sorts the list according to the transaction date.
  void sortByDate();


  /// Appends a list of Transaction objects
  void append(const QList<Transaction *> & trs);
  void append(const QList<AtomicTransaction *> & trs);
  void append(const TransactionPtrList & trs);
  void append(AtomicTransaction * tr);

  /// Returns a pointer to the numbered element.
  ///
  /// This function is necessary to play well in template things
  AtomicTransaction * pointerTo(int nb);

  /// Returns the transactions in the given period
  TransactionPtrList transactionsForPeriod(const Period & period) const;

};

/// This class represents a list of Transaction objects, ready for
/// storage, with a few additional functionalities.
class TransactionList : public WatchableList<Transaction> {
public:

  TransactionList() {;};
  TransactionList(const  QList<Transaction> & l) : 
    WatchableList<Transaction>(l) {;};

  /// Sorts the list according to the transaction date.
  void sortByDate();

  /// Returns a sublist in the same order as this one containing only
  /// transactions dealing with the given account (as per
  /// Account.isSameAccount()).
  TransactionList sublist(const Account & account) const;

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

  /// Mark the transactions as "recent"
  void markRecent() {
    for(int i = 0; i < size(); i++)
      operator[](i).setRecent();
  };

  /// Transforms into a pointer list, for further manipulations.
  ///
  /// If @a subtractions is true (the default), then an
  /// AtomicTransaction is returned for all subtransactions too
  TransactionPtrList toPtrList(bool subtransactions = true);

  // /// Returns a list of Transaction pointers for Transaction within
  // /// the date range. The list is assume to be sorted, at least with
  // /// respect to dates. Both dates are inclusive.
  // ///
  // /// \todo this one should also be part of TransactionPtrList.
  // TransactionPtrList transactionsWithinRange(const QDate & before,
  //       				     const QDate & after);


  /// Returns a pointer to the numbered element
  Transaction * pointerTo(int nb);

  
};

#endif
