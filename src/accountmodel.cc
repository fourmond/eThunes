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
// For the delegate
#include <categorycombo.hh>

QHash<QString, QIcon> AccountModel::statusIcons;

TransactionItem::TransactionItem(Transaction * t) : transaction(t) {
  connect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
          SLOT(transactionChanged()));
}


int TransactionItem::columnCount() const {
  return AccountModel::LastColumn;
}

QVariant TransactionItem::data(int column, int role) const {
  const Transaction *t = transaction;
  if(! t)
    return QVariant();
  if(role == Qt::DisplayRole) {
    switch(column) {
    case AccountModel::DateColumn: return QVariant(t->getDate());
    case AccountModel::AmountColumn: return QVariant(t->getAmountString());
    case AccountModel::BalanceColumn: return QVariant(t->getBalanceString());
    case AccountModel::NameColumn: return QVariant(t->getName());
    case AccountModel::CategoryColumn: return QVariant(t->categoryName());
    case AccountModel::TagsColumn: return QVariant(t->tagString());
    case AccountModel::MemoColumn: return t->getDescription();
    default:
      return QVariant();
    }
  }
  if(role == Qt::EditRole) {
    switch(column) {
    case AccountModel::CategoryColumn: return QVariant(t->categoryName());
    case AccountModel::TagsColumn: return QVariant(t->tagString());
    case AccountModel::LinksColumn: if(t->links.size())
        return t->links.htmlLinkList().join(", ");
      return QVariant();
    default:
      return QVariant();
    }
  }
  if(role == Qt::DecorationRole) {
    if(column == AccountModel::RecentColumn) {
      if(t->isRecent())
        return AccountModel::statusIcon("recent");
      else
        return AccountModel::statusIcon("default");
    }
    return QVariant();
  }
  if(role == Qt::TextAlignmentRole) {
    switch(column) {
    case AccountModel::AmountColumn:
    case AccountModel::BalanceColumn:
      return QVariant(Qt::AlignRight);
    default:
      return QVariant();
    }
  }

  if(role == Qt::FontRole && 
     (column == AccountModel::BalanceColumn 
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
    if(column == AccountModel::DateColumn) {
      QColor color;
      int month = t->getDate().month() - 1;
      color.setHsv(month * 170 % 360, 255, 100);
      return QBrush(color);
    }

    if(t->getCategory() &&
       (column == AccountModel::CategoryColumn ||
        column == AccountModel::NameColumn))
      return QBrush(t->getCategory()->categoryColor());
    if(column == AccountModel::AmountColumn) {
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
};

Qt::ItemFlags TransactionItem::flags(int column) const  
{
  switch(column) {
  case AccountModel::CategoryColumn:
  case AccountModel::TagsColumn:
    return Qt::ItemIsSelectable|
      Qt::ItemIsEnabled|Qt::ItemIsEditable;
  default: return Qt::ItemIsSelectable|
      Qt::ItemIsEnabled;
  }
  return 0;
}

void TransactionItem::transactionChanged() 
{
  emit(itemChanged(this, 0, AccountModel::LastColumn));
}

bool TransactionItem::setData(int column, const QVariant & value,
                                      int role) 
{
  Transaction *t = transaction;
  if(role == Qt::EditRole) {
    switch(column) {
    case AccountModel::CategoryColumn:
      t->setCategoryFromName(value.toString());
      emit(itemChanged(this, column, column));
      return true;
    case AccountModel::TagsColumn:
      t->setTagList(value.toString());
      emit(itemChanged(this, column, column));
      return true;
    default:
      return false;
    }
  }
  return false;
}

void TransactionItem::changeTransaction(Transaction * newt)
{
  if(newt == transaction)
    return;                     // nothing to do!
  disconnect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
             this, SLOT(transactionChanged()));
  transaction = newt;
  connect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
          SLOT(transactionChanged()));
  transactionChanged();
}


//////////////////////////////////////////////////////////////////////
 

TransactionListItem::TransactionListItem(TransactionList * trs) : 
  transactions(trs), transactionsPtr(NULL) 
{
  for(int i = transactions->size(); i > 0; )
      appendChild(new TransactionItem(&(transactions->operator[](--i))));
  connect(transactions->watchDog(), 
          SIGNAL(attributeChanged(const Watchdog *, const QString &)),
          SLOT(onAttributeChanged(const Watchdog *, const QString &)));

  connect(transactions->watchDog(), 
          SIGNAL(objectInserted(const Watchdog *, int, int)),
          SLOT(onObjectInserted(const Watchdog *, int, int)));

  connect(transactions->watchDog(), 
          SIGNAL(objectRemoved(const Watchdog *, int, int)),
          SLOT(onObjectRemoved(const Watchdog *, int, int)));

}

TransactionListItem::TransactionListItem(TransactionPtrList * ptr) : 
  transactions(NULL), transactionsPtr(ptr)
{
  for(int i = 0; i < transactionsPtr->size(); i++ ) {
    Transaction * trs = 
      dynamic_cast<Transaction *>(transactionsPtr->value(i));
    if(trs)
      appendChild(new TransactionItem(trs));
  }
}

QVariant TransactionListItem::data(int column, int role) const
{
    return QVariant();
}



QVariant TransactionListItem::headerData(int section, 
                                         Qt::Orientation /*orientation*/, 
                                         int role) const
{
  if(role == Qt::DisplayRole) {
    switch(section) {
    case AccountModel::DateColumn: return QVariant(tr("Date"));
    case AccountModel::AmountColumn: return QVariant(tr("Amount"));
    case AccountModel::BalanceColumn: return QVariant(tr("Balance"));
    case AccountModel::NameColumn: return QVariant(tr("Name"));
    case AccountModel::LinksColumn: return QVariant(tr("Links"));
    case AccountModel::MemoColumn: return QVariant(tr("Memo"));
    case AccountModel::TagsColumn: return QVariant(tr("Tags"));
    case AccountModel::CategoryColumn: return QVariant(tr("Category"));
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


void TransactionListItem::onAttributeChanged(const Watchdog * wd, 
                                             const QString &name)
{
  if(name == "all") {
    // All the members were probably swapped, so we'll ensure the
    // transactions pointers are matching

    /// @todo assert that the source's size and the current number of
    /// children are the same
    int sz = transactions->size();
    for(int i = 0; i < sz; i++) {
      TransactionItem * it = dynamic_cast<TransactionItem *>(childAt(i));
      if(!it) {
        QTextStream o(stdout);
        o << "Null pointer " << endl;
        continue;               // But shouldn't happen
      }
      it->changeTransaction(&(transactions->operator[](sz - 1 - i)));
    }
    
  }
}

void TransactionListItem::onObjectInserted(const Watchdog * wd, int at, int nb)
{
  int na = children.size() - at - 1;
  emit(rowsWillChange(this, at, nb));
  for(int i = nb - 1; i >= 0 ; i--)
    insertChild(na, new TransactionItem(&(transactions->
                                          operator[](at + nb - i - 1))));
  emit(rowsChanged(this));
}

void TransactionListItem::onObjectRemoved(const Watchdog * wd, int at, int nb)
{
}

Account * TransactionListItem::account() const
{
  if(transactions && transactions->size() > 0)
    return transactions->value(0).account;
  if(transactionsPtr && transactionsPtr->size() > 0)
    return transactionsPtr->value(0)->getAccount();
  return NULL;
}


/// @todo This function can probably move one degree of abstraction
/// higher.
TransactionItem * TransactionListItem::findTransaction(const Transaction * tr)
{
  for(int i = 0; i < children.size(); i++) {
    ModelItem * item = children[i];
    TransactionItem * ti = dynamic_cast<TransactionItem *>(item);
    if(ti) {
      if(ti->getTransaction() == tr)
        return ti;
      else
        continue;
    }

    TransactionListItem * til = dynamic_cast<TransactionListItem *>(item);
    if(til) {
      TransactionItem * ti = til->findTransaction(tr);
      if(ti)
        return ti;
    }
  }
  return NULL;
}



//////////////////////////////////////////////////////////////////////

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
  OOModel(NULL), transactions(t), transactionsPtr(NULL)
{
  setRoot(new TransactionListItem(transactions));
}

AccountModel::AccountModel(TransactionPtrList * t) :
  OOModel(NULL), transactions(NULL), transactionsPtr(t)
{
  setRoot(new TransactionListItem(transactionsPtr));
}

Account * AccountModel::account() const
{

  TransactionListItem * it = rootItem();
  if(it)
    return it->account();
  return NULL;
}

QModelIndex AccountModel::index(Transaction * transaction)
{
  TransactionItem * it = rootItem()->findTransaction(transaction);
  if(it)
    return indexForItem(it);
  return QModelIndex();
}

TransactionListItem * AccountModel::rootItem() const
{
  return dynamic_cast<TransactionListItem *>(root);
}

Transaction * AccountModel::indexedTransaction(QModelIndex index) const
{
  TransactionItem * it = 
    dynamic_cast<TransactionItem *>(item(index));
  if(it)
    return it->getTransaction();
  return NULL;
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

