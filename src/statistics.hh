/**
    \file statistics.hh
    Statistics
    Copyright 2011 by Vincent Fourmond

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


#ifndef __STATISTICS_HH
#define __STATISTICS_HH

#include <transactionlists.hh>

/// Statistics by indexed by top-level Category.
class CategorizedStatistics : public QHash<QString, BasicStatistics> {
public:
  /// Adds a Transaction to the statistics.
  void addTransaction(const Transaction * t);

  class Item {
  public:
    
    QString category;
    int amount;

    Item() {;};

    Item(const QString & c, int a) : category(c), amount(a) {;};

    bool operator<(const Item & b) const {
      return amount < b.amount;
    };
  };

  /// Categorize the transactions, ie returns a sorted list of Item
  QList<Item> categorize() const;
};

class CategorizedMonthlyStatistics : public QHash<int, CategorizedStatistics> {
public:
  /// Adds a Transaction to the statistics.
  void addTransaction(const Transaction * t);
};

/// This class computes up various statistics about a series of
/// transactions: it organizes them by month and top-level categories.
class Statistics {
  /// The target account.
  Account * account;
public:
  /// Monthly statistics. Hashed by monthID();
  CategorizedMonthlyStatistics stats;

  /// Creates statistics from a transaction list.
  Statistics(const TransactionPtrList & lst);

  /// Returns a HTML string suitable for representing statistics of
  /// the given number of months. Returns the stats in form of a HTML
  /// table.
  QString htmlStatistics(int months = 5) const;
};


#endif
