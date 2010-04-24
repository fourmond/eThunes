/** 
    \file categorymodel.hh
    An item model providing statistics about categories for a given wallet.
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


#ifndef __CATEGORYMODEL_HH
#define __CATEGORYMODEL_HH

#include <wallet.hh>

class CategoryModel : public QAbstractItemModel {

  Q_OBJECT;

  /// The Wallet whose categories we'll display.
  Wallet * wallet;

protected:
  

  /// Returns a pointer to the CategoryHash relevant to the given
  /// index (ie subcategories)
  CategoryHash * indexedCategoryHash(QModelIndex index) const;

public:

  /// Returns the category corresponding to the index, or NULL for
  /// root/invalid
  Category * indexedCategory(QModelIndex index) const;

  /// The transactions to be displayed.
  CategoryModel(Wallet * wallet);


  enum {
    NameColumn,
    CurrentMonthColumn, 
    LastMonthColumn, 
    AverageMonthColumn, 
    AmountColumn,
    DebitColumn,
    CreditColumn,
    NumberColumn, 
    LastColumn
  };


  virtual QModelIndex index(int row, int column, 
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual int columnCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation, 
			      int role) const ;

  // virtual Qt::ItemFlags flags(const QModelIndex & index) const;

  // virtual bool setData(const QModelIndex & index, const QVariant & value, 
  // 		       int role = Qt::EditRole);  

public slots:
  
};

#endif
