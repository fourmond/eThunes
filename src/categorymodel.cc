/*
    categorymodel.cc: model for displaying category information
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

#include <headers.hh>
#include <categorymodel.hh>

CategoryModel::CategoryModel(Wallet * w) :
  wallet(w)
{
}

Category * CategoryModel::indexedCategory(QModelIndex index) const
{
  return static_cast<Category *>(index.internalPointer());
}

CategoryHash * CategoryModel::indexedCategoryHash(QModelIndex index) const
{
  Category * c = indexedCategory(index);
  if(c)
    return &c->subCategories;
  else
    return &wallet->categories;
}


QModelIndex CategoryModel::index(int row, int column, 
				 const QModelIndex & parent) const
{
  if(parent.isValid()) {
    CategoryHash * h = indexedCategoryHash(parent);
    if(row >= h->count())
      return QModelIndex();
    QStringList l = h->keys();
    l.sort();
    if(row >= l.count())
      return QModelIndex();
    return createIndex(row, column, 
		       &(h->operator[](l[row])));
  }
  else
    return createIndex(row, column, (void *)NULL);
}

QModelIndex CategoryModel::parent(const QModelIndex & index) const
{
  // printf("Parent: %d %d %d\n", index.row(), index.column(), index.internalId());
  if(index.isValid()) {
    Category * c = indexedCategory(index);
    if(c)
      return createIndex(0,0, c->parent);
    else
      return QModelIndex();
  }
  else
    return QModelIndex();
}

int CategoryModel::rowCount(const QModelIndex & index) const
{
  if(index.isValid()) {
    CategoryHash * h = indexedCategoryHash(index);
    return h->count();
  }
  return 0;
}

int CategoryModel::columnCount(const QModelIndex & /*index*/) const
{  
  return LastColumn;
}

QVariant CategoryModel::headerData(int section, 
				  Qt::Orientation /*orientation*/, 
				  int role) const 
{
  if(role == Qt::DisplayRole) {
    switch(section) {
    // case DateColumn: return QVariant(tr("Date"));
    case AmountColumn: return QVariant(tr("Amount"));
    case NumberColumn: return QVariant(tr("Number"));
    // case BalanceColumn: return QVariant(tr("Balance"));
    case NameColumn: return QVariant(tr("Name"));
    // case MemoColumn: return QVariant(tr("Memo"));
    // case CategoryColumn: return QVariant(tr("Category"));
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


QVariant CategoryModel::data(const QModelIndex& index, int role) const
{
  const Category *c = indexedCategory(index);
  /// \todo Add a cache for results
  if(! c)
    return QVariant();
  if(role == Qt::DisplayRole) {
    TransactionPtrList list = wallet->categoryTransactions(c);
    TransactionListStatistics stats;
    switch(index.column()) {
    case NameColumn: 
      return QVariant(c->name);
    case AmountColumn: 
      stats = list.statistics();
      return QVariant(Transaction::formatAmount(stats.totalAmount));
    case NumberColumn: 
      return QVariant(list.count());
    default:
      return QVariant();
    }
  }
  // if(role == Qt::EditRole) {
  //   switch(index.column()) {
  //   case CategoryColumn: return QVariant(t->categoryName());
  //   default:
  //     return QVariant();
  //   }
  // }
  // if(role == Qt::TextAlignmentRole) {
  //   switch(index.column()) {
  //   case AmountColumn:
  //   case BalanceColumn:
  //     return QVariant(Qt::AlignRight);
  //   default:
  //     return QVariant();
  //   }
  // }
  // if(role == Qt::FontRole && index.column() == BalanceColumn) {
  //   QFont font;
  //   font.setBold(true);
  //   return font;
  // }
  // if(role == Qt::BackgroundRole) {
  //   QColor background;
  //   int month = t->date.month() - 1;
  //   background.setHsv(month * 170 % 360, 50, (index.row() % 2 ? 220 : 240));
  //   return QBrush(background);
  // }
  // if(role == Qt::ForegroundRole) {
  //   if(t->category)
  //     return QBrush(t->category->categoryColor());
  //   return QVariant();
  // }
  /// \todo Add colors here !
  else
    return QVariant();
}

// Qt::ItemFlags CategoryModel::flags(const QModelIndex & index) const
// {
//   if(index.isValid()) {
//     switch(index.column()) {
//     case CategoryColumn: return Qt::ItemIsSelectable|
// 	Qt::ItemIsEnabled|Qt::ItemIsEditable;
//     default: return Qt::ItemIsSelectable|
// 	Qt::ItemIsEnabled;
//     }
//   }
//   return 0;
// }

// bool CategoryModel::setData(const QModelIndex & index, const QVariant & value, 
// 			   int role)
// {
//   // Transaction *t = indexedTransaction(index);
//   // if(index.column() == CategoryColumn && t && role == Qt::EditRole) {
//   //   t->setCategoryFromName(value.toString());
//   //   emit(dataChanged(index, index));
//   //   return true;
//   // }
//   // return false;
// }


