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
  void addTransaction(const AtomicTransaction * t, bool topLevel = true);

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


/// This abstract class handles categorized statistics
class PeriodicCategorizedStatistics : 
  public QHash<int, CategorizedStatistics> {
protected:
  Account * account;

  Wallet * wallet;
public:

  PeriodicCategorizedStatistics(Account * ac);

  /// Adds a Transaction to the statistics.
  virtual void addTransaction(const AtomicTransaction * t, 
                              bool topLevel = true) = 0;

  /// The name for one element (hopefully crosslinked)
  virtual QString elementName(int id) const = 0;

  /// Formats a category name
  virtual QString categoryName(int id, const QString & name,
                               const QString & display = "") const = 0;

  /// The number of months comprized in the display (for averaging
  /// purposes)
  ///
  /// @todo Finish that !
  virtual int monthNumber() const {
    return 1;
  };
};


/// Represents statistics based on a month
class CategorizedMonthlyStatistics : public PeriodicCategorizedStatistics {
public:

  CategorizedMonthlyStatistics(Account * ac) : 
    PeriodicCategorizedStatistics(ac) {;};
  virtual void addTransaction(const AtomicTransaction * t, 
                              bool topLevel = true);
  virtual QString elementName(int id) const;
  virtual QString categoryName(int id,
                               const QString & category, 
                               const QString & display = "") const;
};

/// Trimester-based statistics. The hash is the monthID() of the first
/// month.
class CategorizedTrimesterStatistics : public PeriodicCategorizedStatistics {
public:
  CategorizedTrimesterStatistics(Account * ac) : 
    PeriodicCategorizedStatistics(ac) {;};

  virtual void addTransaction(const AtomicTransaction * t, 
                              bool topLevel = true);
  virtual QString elementName(int id) const;
  virtual QString categoryName(int id,
                               const QString & category, 
                               const QString & display = "") const;

  virtual int monthNumber() const {
    return 3;
  };

};

/// Trimester-based statistics. The hash is the year
class CategorizedYearlyStatistics : public PeriodicCategorizedStatistics {
public:
  CategorizedYearlyStatistics(Account * ac) : 
    PeriodicCategorizedStatistics(ac) {;};
  virtual void addTransaction(const AtomicTransaction * t, 
                              bool topLevel = true);
  virtual QString elementName(int id) const;
  virtual QString categoryName(int id,
                               const QString & category, 
                               const QString & display = "") const;

  virtual int monthNumber() const {
    return 12;
  };
};

/// This class computes up various statistics about a series of
/// transactions: it organizes them by month and top-level categories.
class Statistics {
  /// The target account.
  Account * account;

protected:

  QString htmlStatistics(PeriodicCategorizedStatistics * which, int months = 5,
                         int maxDisplay = 6, bool monthlyAverage = false) const;

public:

  enum Period {
    Monthly = 0,
    Trimester = 1,
    Yearly = 2
  };

  /// All different statistics
  QList<PeriodicCategorizedStatistics *> stats;

  /// Creates statistics from a transaction list.
  Statistics(const TransactionPtrList & lst, bool topLevel = true);




  /// Returns a HTML string suitable for representing statistics of
  /// the given number of months. Returns the stats in form of a HTML
  /// table. If months is negative, returns the stats for all the
  /// months.
  ///
  /// @todo Idea: graphics display (possibly by alternance) where
  /// each data point would be very visible, with a neat tooltip and a
  /// context menu for showing transactions ?
  QString htmlStatistics(Period period = Monthly, int months = 5, 
                         int maxDisplay = 6, 
                         bool monthlyAverage = false) const;

  ~Statistics();
};


#endif
