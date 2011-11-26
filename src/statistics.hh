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

class Category;

/// Statistics by indexed by a Category (either top-level or not)
class CategorizedStatistics : public QHash<const Category *, BasicStatistics> {
public:
  /// Adds a Transaction to the statistics.
  void addTransaction(const Transaction * t, bool topLevel = true);

  class Item {
  public:
    
    QString category;
    int amount;

    Item() {;};

    Item(const QString & c, int a) : category(c), amount(a) {;};
    Item(const Category * cat, int a) : amount(a) {
      if(cat)
        category = cat->fullName();
      else
        category = "(uncategorized)";
    };

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
  void addTransaction(const Transaction * t, bool topLevel = true);
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
  Statistics(const TransactionPtrList & lst, bool topLevel = true);

  /// Returns a HTML string suitable for representing statistics of
  /// the given number of months. Returns the stats in form of a HTML
  /// table. If months is negative, returns the stats for all the
  /// months.
  ///
  /// Ideas: @li have aggregated stats configurable on a given number
  /// of months
  QString htmlStatistics(int months = 5, int maxDisplay = 6) const;
};


#endif
