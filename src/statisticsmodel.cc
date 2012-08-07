/*
    statisticsmodel.cc: model for displaying statistics about transaction lists
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

#include <headers.hh>
#include <statisticsmodel.hh>

StatisticsModel::StatisticsModel()
{
  // nothing to do for now.
}

int StatisticsModel::columnCount(const QModelIndex & /*index*/) const
{
  return LastColumn;
}

TransactionListStatistics * StatisticsModel::statsForItem(const QModelIndex & idx) const
{
  TransactionPtrList * transactions = transactionsForItem(idx);
  if(! transactions)
    return NULL;

  if(stats.contains(transactions))
    return stats[transactions];

  TransactionListStatistics * s = 
    new TransactionListStatistics(transactions->statistics());
  stats.insert(transactions, s);
  return s;
}


QVariant StatisticsModel::headerData(int section,
				  Qt::Orientation /*orientation*/,
				  int role) const
{
  QDate now = QDate::currentDate();
  if(role == Qt::DisplayRole) {
    switch(section) {
    case AmountColumn: return tr("Total");
    case NumberColumn: return tr("Number");
    case NameColumn: return tr("Name");
    case CreditColumn: return tr("Total credit");
    case DebitColumn: return tr("Total debit");

    case CurrentMonthColumn: 
      return QVariant(now.toString(tr("MMM yy")));
    case LastMonthColumn: 
      return QVariant(now.addMonths(-1).toString(tr("MMM yy")));
    case MonthBeforeColumn: 
      return QVariant(now.addMonths(-2).toString(tr("MMM yy")));
    case AverageMonthColumn: return QVariant(tr("Monthly average"));

    case CurrentYearColumn: 
      return QString("%1").arg(now.year());
    case LastYearColumn: 
      return QString("%1").arg(now.year()-1);
    case YearBeforeColumn: 
      return QString("%1").arg(now.year()-2);
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


QVariant StatisticsModel::data(const QModelIndex& index, int role) const
{
  TransactionListStatistics * stats = statsForItem(index);
  if(! stats)
    return QVariant();
  QDate now = QDate::currentDate();

  if(role == Qt::DisplayRole) {
    switch(index.column()) {
    case AmountColumn:
      return formatAmount(stats->totalAmount);
    case CreditColumn:
      return formatAmount(stats->totalCredit);
    case DebitColumn:
      return formatAmount(stats->totalDebit);

    case CurrentMonthColumn:
      return formatAmount(stats->thisMonthStats().totalAmount);
    case LastMonthColumn:
      return formatAmount(stats->lastMonthStats().totalAmount);
    case MonthBeforeColumn:
      return formatAmount(stats->monthlyStats[Transaction::thisMonthID() - 2].totalAmount);
    case AverageMonthColumn:
      return formatAmount(stats->monthlyAverageAmount());

    case CurrentYearColumn:
      return formatAmount(stats->yearlyStats[now.year()].totalAmount);
    case LastYearColumn:
      return formatAmount(stats->yearlyStats[now.year()-1].totalAmount);
    case YearBeforeColumn:
      return formatAmount(stats->yearlyStats[now.year()-2].totalAmount);

    case NumberColumn:
      return stats->number;
    default:
      return QVariant();
    }
  }
  if(role == Qt::TextAlignmentRole) {
    switch(index.column()) {
    case AmountColumn:
    case CurrentMonthColumn:
    case LastMonthColumn:
    case AverageMonthColumn:
    case CurrentYearColumn:
    case LastYearColumn:
    case DebitColumn:
    case CreditColumn:
    case MonthBeforeColumn:
    case YearBeforeColumn:
      return QVariant(Qt::AlignRight);
    default:
      return QVariant();
    }
  }
  return QVariant();
}


//////////////////////////////////////////////////////////////////////

CategoryModel::CategoryModel(Wallet * w) :
  wallet(w)
{
}

Category * CategoryModel::indexedCategory(const QModelIndex &index) const
{
  return static_cast<Category *>(index.internalPointer());
}

CategoryHash * CategoryModel::indexedCategoryHash(const QModelIndex &index) const
{
  Category * c = indexedCategory(index);
  if(c)
    return &c->subCategories;
  else
    return &wallet->categories;
}

TransactionPtrList * CategoryModel::transactionsForItem(const QModelIndex & idx) const
{
  Category * c = indexedCategory(idx);
  if(! c)
    return NULL;

  /// @todo Cache never gets invalidated... That's pretty bad ;-)...
  if(cachedLists.contains(c)) 
    return cachedLists[c];

  TransactionPtrList * trs = 
    new TransactionPtrList(wallet->categoryTransactions(c));
  cachedLists.insert(c, trs);
  return trs;
  
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

QVariant CategoryModel::data(const QModelIndex& index, int role) const
{
  if(index.column() == StatisticsModel::NameColumn) {
    const Category *c = indexedCategory(index);
    if(! c)
      return QVariant();
    switch(role) {
    case Qt::DisplayRole:
      return c->name;
    case Qt::ForegroundRole:
      return QBrush(c->categoryColor());
    default:
      return QVariant();
    }
    return QVariant();          // useless but required by g++
  }
  else 
    return StatisticsModel::data(index, role);
}




//////////////////////////////////////////////////////////////////////

TagModel::TagModel(Wallet * w) :
  wallet(w)
{
}

Tag * TagModel::indexedTag(const QModelIndex &index) const
{
  return static_cast<Tag *>(index.internalPointer());
}

TagHash * TagModel::indexedTagHash(const QModelIndex &index) const
{
  Tag * c = indexedTag(index);
  if(c)
    return NULL;
  else
    return &wallet->tags;
}

TransactionPtrList * TagModel::transactionsForItem(const QModelIndex & idx) const
{
  Tag * c = indexedTag(idx);
  if(! c)
    return NULL;

  /// @todo Cache never gets invalidated... That's pretty bad ;-)...
  if(cachedLists.contains(c)) 
    return cachedLists[c];

  TransactionPtrList * trs = 
    new TransactionPtrList(wallet->taggedTransactions(c));
  cachedLists.insert(c, trs);
  return trs;
  
}


QModelIndex TagModel::index(int row, int column,
				 const QModelIndex & parent) const
{
  if(parent.isValid()) {
    TagHash * h = indexedTagHash(parent);
    if(! h)
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

QModelIndex TagModel::parent(const QModelIndex & index) const
{
  if(index.isValid()) {
    Tag * t = indexedTag(index);
    if(t)
      return createIndex(0,0, (void *)NULL);
    return QModelIndex();
  }
  else
    return QModelIndex();
}

int TagModel::rowCount(const QModelIndex & index) const
{
  if(index.isValid()) {
    TagHash * h = indexedTagHash(index);
    if(h)
      return h->count();
    else
      return 0;
  }
  return 0;
}

QVariant TagModel::data(const QModelIndex& index, int role) const
{
  if(index.column() == StatisticsModel::NameColumn) {
    const Tag *c = indexedTag(index);
    if(! c)
      return QVariant();
    switch(role) {
    case Qt::DisplayRole:
      return c->name;
    case Qt::ForegroundRole:
      //      return QBrush(c->categoryColor());
    default:
      return QVariant();
    }
    return QVariant();          // useless but required by g++
  }
  else 
    return StatisticsModel::data(index, role);
}




