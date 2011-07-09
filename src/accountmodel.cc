/*
    accountmodel.cc: model for transaction lists
    Copyright 2010, 2011 by Vincent Fourmond

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

#include <htlabel.hh>
#include <linkshandler.hh>
// For the delegate
#include <categorycombo.hh>

QHash<QString, QIcon> AccountModel::statusIcons;

/// @todo This class should be rewritten with object-oriented things
/// in mind. Items should be objects with appropriate virtual methods.
/// This would allow painlessly to have objects for months only. That
/// would also allow easily to follow objects:
/// \li this way, each object watches the target object
/// \li we don't invalidate the whole view each time a category
/// changes, which is very expensive.

const QIcon & AccountModel::statusIcon(const QString & status)
{
  if(! statusIcons.contains(status)) {
    if(QFile::exists("icons:accountmodel-" + status + ".png"))
      statusIcons[status] = QIcon("icons:accountmodel-" + status + ".png");
    else
      statusIcons[status] = QIcon(QPixmap(16,16));
  }
  return statusIcons[status];
}

AccountModel::AccountModel(TransactionList * t) :
  transactions(t), transactionsPtr(NULL)
{
  connect(*t, SIGNAL(changed(const Watchdog *)),
          SLOT(accountChanged())); 
}

AccountModel::AccountModel(TransactionPtrList * t) :
  transactions(NULL), transactionsPtr(t)
{
  connect(*t, SIGNAL(changed(const Watchdog *)),
          SLOT(accountChanged())); 
}

Transaction * AccountModel::indexedTransaction(int idx) const
{
  if(transactions) {
    if(idx < transactions->size())
      return const_cast<Transaction *>(& (*transactions)[transactions->count() - idx - 1]);

    else
      return NULL;
  }
  else {
    if(idx < transactionsPtr->size())
      return transactionsPtr->operator[](idx);
    else
      return NULL;
  }
}

Account * AccountModel::account() const
{
  Transaction * first = indexedTransaction(0);
  if(first)
    return first->account;
  return NULL;
}

QModelIndex AccountModel::index(Transaction * transaction)
{
  Transaction * t;
  int i = 0;
  do {
    t = indexedTransaction(i);
    if(t == transaction) {
      return createIndex(i, 0, i);
    }
    i++;
  } while(t);
  return QModelIndex();
}


Transaction * AccountModel::indexedTransaction(QModelIndex index) const
{
  if(index.isValid() && index.internalId() >= 0)
    return indexedTransaction(index.internalId());
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
      return transactionCount();
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
    case LinksColumn: return QVariant(tr("Links"));
    case MemoColumn: return QVariant(tr("Memo"));
    case TagsColumn: return QVariant(tr("Tags"));
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
    case DateColumn: return QVariant(t->getDate());
    case AmountColumn: return QVariant(t->getAmountString());
    case BalanceColumn: return QVariant(t->getBalanceString());
    case NameColumn: return QVariant(t->getName());
    case CategoryColumn: return QVariant(t->categoryName());
    case TagsColumn: return QVariant(t->tagString());
    case MemoColumn: return t->getDescription();
    default:
      return QVariant();
    }
  }
  if(role == Qt::EditRole) {
    switch(index.column()) {
    case CategoryColumn: return QVariant(t->categoryName());
    case TagsColumn: return QVariant(t->tagString());
    case LinksColumn: if(t->links.size())
	return t->links.htmlLinkList().join(", ");
      return QVariant();
    default:
      return QVariant();
    }
  }
  if(role == Qt::DecorationRole) {
    if(index.column() == RecentColumn) {
      if(t->isRecent())
	return statusIcon("recent");
      else
	return statusIcon("default");
    }
    return QVariant();
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

  if(role == Qt::FontRole && 
     (index.column() == BalanceColumn 
      || t->isRecent())) {
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
      int month = t->getDate().month() - 1;
      color.setHsv(month * 170 % 360, 255, 100);
      return QBrush(color);
    }

    if(t->getCategory() &&
       (index.column() == CategoryColumn ||
	index.column() == NameColumn))
      return QBrush(t->getCategory()->categoryColor());
    if(index.column() == AmountColumn) {
      QColor color;
      /// \todo Provide customization of the colors
      if(t->getAmount() < 0)
	color.setHsv(240,200,130);
      else
	color.setHsv(120,200,130);
      return QBrush(color);
    }
    return QVariant();
  }
  else
    return QVariant();
}

Qt::ItemFlags AccountModel::flags(const QModelIndex & index) const
{
  if(index.isValid()) {
    switch(index.column()) {
    case CategoryColumn:
    case TagsColumn:
      return Qt::ItemIsSelectable|
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
  if(t && role == Qt::EditRole) {
    switch(index.column()) {
    case CategoryColumn:
      t->setCategoryFromName(value.toString());
      emit(dataChanged(index, index));
      return true;
    case TagsColumn:
      t->setTagList(value.toString());
      emit(dataChanged(index, index));
      // if(t->account && t->account->wallet)
      // 	t->account->wallet->didChangeCategories();
      return true;
    default:
      return false;
    }
  }
  return false;
}


void AccountModel::accountChanged()
{
  int size = transactions ? transactions->size() : 
    transactionsPtr->size();
  emit(dataChanged(index(0,0, index(0,0,QModelIndex())),
		   index(size-1,LastColumn-1,
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
  CategoryCombo * box = new CategoryCombo(wallet, parent);
  return box;
}

void AccountItemDelegate::setEditorData(QWidget * editor,
					const QModelIndex & index) const
{
  static_cast<CategoryCombo*>(editor)
    ->setEditText(index.data(Qt::EditRole).toString());
}


void AccountItemDelegate::setModelData(QWidget * editor,
				       QAbstractItemModel * model,
				       const QModelIndex & index) const
{
  model->setData(index, static_cast<CategoryCombo*>(editor)->currentText());
}


///////////////////////////////////////////////////////////////


QWidget * LinksItemDelegate::createEditor(QWidget * parent,
					  const QStyleOptionViewItem & /*option*/,
					  const QModelIndex & /*index*/ ) const
{
  return new HTLabel(parent);
}

void LinksItemDelegate::setEditorData(QWidget * editor,
				      const QModelIndex & index) const
{
  HTLabel * label = static_cast<HTLabel*>(editor);
  label->setText(index.data(Qt::EditRole).toString());
}


void LinksItemDelegate::setModelData(QWidget * /*editor*/,
				     QAbstractItemModel * /*model*/,
				     const QModelIndex & /*index*/) const
{
}

