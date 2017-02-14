/**
    \file statisticsmodel.hh
    An item model providing statistics about list of transactions of any kind, and its declination into categories and tags
    Copyright 2010, 2012 by Vincent Fourmond

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


#ifndef __STATISTICSMODEL_HH
#define __STATISTICSMODEL_HH

#include <wallet.hh>

/// This superclass simply handles the display of statistics about
/// item lists. As far as the first column isn't concerned, the
/// derived classes just have to reimplement 
class StatisticsModel : public QAbstractItemModel {

  Q_OBJECT;


protected:
  /// Formats the amounts using Transaction::formatAmount, although
  /// returns an empty string on 0 value.
  ///
  /// @todo make that configurable
  QString formatAmount(int amount) const {
    if(amount)
      return Transaction::formatAmount(amount);
    return QString("");
  };
  
  /// A cache for the statistics. Based on the pointer value, with the
  /// implicit assumption that objects are in fact immutable...
  mutable QCache<TransactionPtrList *, TransactionListStatistics> stats;

  /// Gets the stats for the given item, caching them if necessary
  /// (NULL is returned in case of problems). 
  TransactionListStatistics * statsForItem(const QModelIndex & idx) const;

public:

  StatisticsModel();
  
  /// Returns a pointer to the list corresponding to the given index,
  /// or NULL if there is no list. Doesn't take ownership of the list
  /// (but caches the results, of course...)
  virtual TransactionPtrList * transactionsForItem(const QModelIndex & idx) const = 0;
  


  enum {
    NameColumn,
    Running7DaysColumn,
    Running30DaysColumn,
    CurrentMonthColumn,
    LastMonthColumn,
    MonthBeforeColumn,
    AverageMonthColumn,
    CurrentYearColumn,
    LastYearColumn,
    YearBeforeColumn,
    AmountColumn,
    DebitColumn,
    CreditColumn,
    NumberColumn,
    LastColumn
  };


  virtual int columnCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
			      int role) const ;

};

/////////////////////////////////////////////////////////////////////////

/// Derived class handling specifically categories.
class CategoryModel : public StatisticsModel {

  Q_OBJECT;

  /// The Wallet whose categories we'll display.
  Wallet * wallet;

protected:


  /// Returns a pointer to the CategoryHash relevant to the given
  /// index (ie subcategories)
  CategoryHash * indexedCategoryHash(const QModelIndex &index) const;

  /// A cache for the transaction lists
  mutable QCache<Category *, TransactionPtrList> cachedLists;

public:

  virtual TransactionPtrList * transactionsForItem(const QModelIndex & idx) const;

  /// Returns the category corresponding to the index, or NULL for
  /// root/invalid
  Category * indexedCategory(const QModelIndex &index) const;

  /// The transactions to be displayed.
  CategoryModel(Wallet * wallet);

  virtual QModelIndex index(int row, int column,
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

public slots:

};

/////////////////////////////////////////////////////////////////////////

/// Derived class handling specifically tags.
class TagModel : public StatisticsModel {

  Q_OBJECT;

  /// The Wallet whose categories we'll display.
  Wallet * wallet;

protected:


  /// A cache for the transaction lists
  mutable QCache<Tag *, TransactionPtrList> cachedLists;

  TagHash * indexedTagHash(const QModelIndex &index) const;

public:

  virtual TransactionPtrList * transactionsForItem(const QModelIndex & idx) const;

  /// Returns the category corresponding to the index, or NULL for
  /// root/invalid
  Tag * indexedTag(const QModelIndex &index) const;

  /// The transactions to be displayed.
  TagModel(Wallet * wallet);

  virtual QModelIndex index(int row, int column,
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

public slots:

};

#endif
