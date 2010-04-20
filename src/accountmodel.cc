/*
    account-model.cc: model for transaction lists
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
#include <account.hh>
#include <accountmodel.hh>
#include <wallet.hh>

AccountModel::AccountModel(TransactionList * t) :
  transactions(t)
{
  if(transactions->size() > 0) {
    if((*transactions)[0].account && (*transactions)[0].account->wallet)
      connect((*transactions)[0].account->wallet, 
	      SIGNAL(accountsChanged()),
	      SLOT(accountChanged())); /// \todo This is suboptimal, but better than nothing
  }
}

Transaction * AccountModel::indexedTransaction(QModelIndex index) const
{
  if(index.isValid() && index.internalId() >= 0 && 
     index.internalId() < transactions->count() )
    return const_cast<Transaction *>(& (*transactions)[transactions->count() - index.internalId() - 1]);
  return NULL;
}

QModelIndex AccountModel::index(int row, int column, 
					const QModelIndex & parent) const
{
  // printf("Index: %d %d %d\n", row, column, parent.internalId());
  if(parent.isValid()) 
    return createIndex(row, column, row);
  else
    return createIndex(row, column, -1);
}

QModelIndex AccountModel::parent(const QModelIndex & index) const
{
  // printf("Parent: %d %d %d\n", index.row(), index.column(), index.internalId());
  if(index.isValid()) {
    if(index.internalId() >= 0) 
      return createIndex(0, 0, -1);
    else
      return QModelIndex();
  }
  else
    return QModelIndex();
}

int AccountModel::rowCount(const QModelIndex & index) const
{
  // printf("row: %d %d %d\n", index.row(), index.column(), index.internalId());
  if(index.isValid()) {
    if(index.internalId() >= 0) 
      return 0;
    else 
      return transactions->count();
  }
  return 0;
}

int AccountModel::columnCount(const QModelIndex & /*index*/) const
{  
  return LastColumn;
}

QVariant AccountModel::headerData(int section, 
				  Qt::Orientation /*orientation*/, 
				  int role) const 
{
  if(role == Qt::DisplayRole) {
    switch(section) {
    case DateColumn: return QVariant(tr("Date"));
    case AmountColumn: return QVariant(tr("Amount"));
    case BalanceColumn: return QVariant(tr("Balance"));
    case NameColumn: return QVariant(tr("Name"));
    case MemoColumn: return QVariant(tr("Memo"));
    case CategoryColumn: return QVariant(tr("Category"));
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


QVariant AccountModel::data(const QModelIndex& index, int role) const
{
  const Transaction *t = indexedTransaction(index);
  if(! t)
    return QVariant();
  if(role == Qt::DisplayRole) {
    switch(index.column()) {
    case DateColumn: return QVariant(t->date);
    case AmountColumn: return QVariant(Transaction::formatAmount(t->amount));
    case BalanceColumn: return QVariant(Transaction::formatAmount(t->balance));
    case NameColumn: return QVariant(t->name);
    case CategoryColumn: return QVariant(t->categoryName());
    case MemoColumn: if(!t->memo.isEmpty())
	return QVariant(t->memo);
      else if(!t->checkNumber.isEmpty())
	return QVariant(tr("Check: %1").arg(t->checkNumber));
    default:
      return QVariant();
    }
  }
  if(role == Qt::EditRole) {
    switch(index.column()) {
    case CategoryColumn: return QVariant(t->categoryName());
    default:
      return QVariant();
    }
  }
  if(role == Qt::TextAlignmentRole) {
    switch(index.column()) {
    case AmountColumn:
    case BalanceColumn:
      return QVariant(Qt::AlignRight);
    default:
      return QVariant();
    }
  }
  if(role == Qt::FontRole && index.column() == BalanceColumn) {
    QFont font;
    font.setBold(true);
    return font;
  }
  // if(role == Qt::BackgroundRole) {
  //   QColor background;
  //   int month = t->date.month() - 1;
  //   background.setHsv(month * 170 % 360, 0, 
  // 		      (index.row() % 2 ? 220 : 240));
  //   return QBrush(background);
  // }
  if(role == Qt::ForegroundRole) {
    if(index.column() == DateColumn) {
      QColor color;
      int month = t->date.month() - 1;
      color.setHsv(month * 170 % 360, 255, 100);
      return QBrush(color);
    }

    if(t->category && 
       (index.column() == CategoryColumn || 
	index.column() == NameColumn))
      return QBrush(t->category->categoryColor());
    if(index.column() == AmountColumn) {
      QColor color;
      /// \todo Provide customization of the colors
      if(t->amount < 0)
	color.setHsv(240,200,130);
      else
	color.setHsv(120,200,130);
      return QBrush(color);
    }
    return QVariant();
  }
  /// \todo Add colors here !
  else
    return QVariant();
}

Qt::ItemFlags AccountModel::flags(const QModelIndex & index) const
{
  if(index.isValid()) {
    switch(index.column()) {
    case CategoryColumn: return Qt::ItemIsSelectable|
	Qt::ItemIsEnabled|Qt::ItemIsEditable;
    default: return Qt::ItemIsSelectable|
	Qt::ItemIsEnabled;
    }
  }
  return 0;
}

bool AccountModel::setData(const QModelIndex & index, const QVariant & value, 
			   int role)
{
  Transaction *t = indexedTransaction(index);
  if(index.column() == CategoryColumn && t && role == Qt::EditRole) {
    t->setCategoryFromName(value.toString());
    emit(dataChanged(index, index));
    if(t->account && t->account->wallet)
      t->account->wallet->didChangeCategories();
    return true;
  }
  return false;
}


void AccountModel::accountChanged()
{
  emit(dataChanged(index(0,0, index(0,0,QModelIndex())),
		   index(transactions->size()-1,LastColumn-1, 
			 index(0,0,QModelIndex()))));
}


///////////////////////////////////////////////////////////////

AccountItemDelegate::AccountItemDelegate(Wallet * w) :
  wallet(w)
{
}

QWidget * AccountItemDelegate::createEditor(QWidget * parent, 
					    const QStyleOptionViewItem & /*option*/, 
					    const QModelIndex & /*index*/ ) const
{
  QComboBox * box = new QComboBox(parent);
  box->setEditable(true);
  if(wallet)
    box->addItems(wallet->categories.categoryNames());
  return box;
}

void AccountItemDelegate::setEditorData(QWidget * editor, 
					const QModelIndex & index) const
{
  QComboBox * box = static_cast<QComboBox*>(editor);
  box->setEditText(index.data(Qt::EditRole).toString());
}


void AccountItemDelegate::setModelData(QWidget * editor, 
				       QAbstractItemModel * model, 
				       const QModelIndex & index) const
{
  QComboBox * box = static_cast<QComboBox*>(editor);
  model->setData(index, box->currentText());
}

