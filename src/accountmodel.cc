/*
    accountmodel.cc: model for transaction lists
    Copyright 2010, 2011, 2012 by Vincent Fourmond

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


/// Returns the data for the given transaction
static QVariant transactionData(AtomicTransaction * t, 
                                int column, int role, bool full = true)
{
  if(! t)
    return QVariant();
  if(role == Qt::DisplayRole) {
    switch(column) {
    case AccountModel::DateColumn: return QVariant(t->getDate());
    case AccountModel::AmountColumn: 
      if(full)
        return Transaction::formatAmount(t->getTotalAmount());
      else
        return Transaction::formatAmount(t->getAmount());
    case AccountModel::BalanceColumn: 
      if(full)
        return QVariant(Transaction::formatAmount(t->getBalance()));
      else
        return QVariant();
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
    if(full && column == AccountModel::RecentColumn) {
      Transaction * tr = dynamic_cast<Transaction*>(t);
      if(tr) {
        if(tr->isRecent())
          return AccountModel::statusIcon("recent");
        else
          return AccountModel::statusIcon("default");
      }
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

  if(role == Qt::FontRole) {
    bool tst = column == AccountModel::BalanceColumn;
    if(! tst && full) {
      // check if full transaction
      Transaction * tr = dynamic_cast<Transaction*>(t);
      tst = tr->isRecent();
    }
    if(tst) {
      QFont font;
      font.setBold(true);
      return font;
    }
    return QVariant();
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
}

LeafTransactionItem::LeafTransactionItem(AtomicTransaction * t) : 
  transaction(t) {
  connect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
          SLOT(transactionChanged()));
}


int LeafTransactionItem::columnCount() const {
  return AccountModel::LastColumn;
}

/// @todo There is quite a lot of code shared between this and
/// FullTransactionItem::data. Maybe have that shared ?
QVariant LeafTransactionItem::data(int column, int role) const {
  return transactionData(transaction, column, role, false);
}

Qt::ItemFlags LeafTransactionItem::flags(int column) const  
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

void LeafTransactionItem::transactionChanged() 
{
  emit(itemChanged(this, 0, AccountModel::LastColumn));
}

/// @todo Lots of code shared too with FullTransaction here. Two
/// approaches: shared (static) functions or variable-less base class.
bool LeafTransactionItem::setData(int column, const QVariant & value,
                                  int role) 
{
  AtomicTransaction *t = transaction;
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

void LeafTransactionItem::changeTransaction(AtomicTransaction * newt)
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


FullTransactionItem::FullTransactionItem(Transaction * t) : 
  transaction(t) {
  transactionConnect();
  ensureHasChildren();
}

void FullTransactionItem::transactionConnect()
{
  connect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
          SLOT(transactionChanged()));
  connect(transaction->subTransactions.watchDog(), 
          SIGNAL(objectInserted(const Watchdog *, int, int)), 
          SLOT(onObjectInserted(const Watchdog *, int, int)));
  connect(transaction->subTransactions.watchDog(), 
          SIGNAL(objectRemoved(const Watchdog *, int, int)), 
          SLOT(onObjectRemoved(const Watchdog *, int, int)));
}

void FullTransactionItem::transactionDisconnect()
{
  disconnect(transaction->watchDog(), SIGNAL(changed(const Watchdog *)), 
             this,
             SLOT(transactionChanged()));
  disconnect(transaction->subTransactions.watchDog(), 
             SIGNAL(objectInserted(const Watchdog *, int, int)), 
             this,
             SLOT(onObjectInserted(const Watchdog *, int, int)));
  disconnect(transaction->subTransactions.watchDog(), 
             SIGNAL(objectRemoved(const Watchdog *, int, int)), 
             this,
             SLOT(onObjectRemoved(const Watchdog *, int, int)));
}


int FullTransactionItem::rootColumns() const
{
  return AccountModel::LastColumn;
}


QVariant FullTransactionItem::data(int column, int role) const {
  return transactionData(transaction, column, role, true);
}

void FullTransactionItem::ensureHasChildren()
{
  if(transaction->subTransactions.size() > 0) {
    int idx = 0;
    for(Transaction::iterator i = transaction->begin();
        i != transaction->end();
        ++i) {
      if(idx < children.size()) {
        LeafTransactionItem * it = 
          dynamic_cast<LeafTransactionItem *>(children[idx]);
        it->changeTransaction(*i);
      }
      else
        appendChild(new LeafTransactionItem(*i));
      ++idx;
    }
  }
}

void FullTransactionItem::onAttributeChanged(const Watchdog * wd, const QString &name)
{
  // What to do here ?
}

void FullTransactionItem::onObjectInserted(const Watchdog * wd, int at, int nb)
{
  QTextStream o(stdout);
  o << "object inserted: " << at << " -- " << nb << endl;
  ensureHasChildren();
}

void FullTransactionItem::onObjectRemoved(const Watchdog * wd, int at, int nb)
{
  ensureHasChildren();
}


void FullTransactionItem::changeTransaction(Transaction * newt)
{
  if(newt == transaction)
    return;                     // nothing to do!
  transactionDisconnect();
  transaction = newt;
  ensureHasChildren();
  transactionConnect();
  transactionChanged();
}

void FullTransactionItem::transactionChanged() 
{
  emit(itemChanged(this, 0, AccountModel::LastColumn));
}

QVariant FullTransactionItem::headerData(int section, 
                                         Qt::Orientation /*orientation*/, 
                                         int role) const
{
  return QVariant();
}


//////////////////////////////////////////////////////////////////////
 
QVariant BaseTransactionListItem::data(int column, int role) const
{
    return QVariant();
}


QVariant BaseTransactionListItem::headerData(int section, 
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

void BaseTransactionListItem::onAttributeChanged(const Watchdog * wd, const QString &name)
{
  effAttributeChanged(wd, name);
}

void BaseTransactionListItem::onObjectInserted(const Watchdog * wd, int at, int nb)
{
  effObjectInserted(wd, at, nb);
}

void BaseTransactionListItem::onObjectRemoved(const Watchdog * wd, int at, int nb)
{
  effObjectRemoved(wd, at, nb);
}


//////////////////////////////////////////////////////////////////////

/// ModelItem (sub)child representing a list of transactions. All
/// objects holding one or more transactions should probably come from
/// this ? 
///
/// @todo Come up with a child of that holding month-based lists of
/// transactions ?
///
/// @todo have a base class for that to minimize the number of
/// functions instantiated
template <class Type, class Item, class Holder> 
class TransactionListItem : public BaseTransactionListItem {
  Holder * list;

public:
  TransactionListItem(Holder * list);

  /// Returns the account linked to the transactions (ie the first one
  /// found)
  virtual Account * account() const;

  /// Looks for a given transaction, and returns the corresponding item
  ///
  ///  (make that return a ModelItem ?) or split the class ?
  virtual ModelItem * findTransaction(const AtomicTransaction * t);
  virtual ModelItem * findTransaction(const Transaction * t);

protected:
  virtual void effAttributeChanged(const Watchdog * wd, const QString &name);
  virtual void effObjectInserted(const Watchdog * wd, int at, int nb);
  virtual void effObjectRemoved(const Watchdog * wd, int at, int nb);
};

template <class Type, class Item, class Holder> 
TransactionListItem<Type, Item, Holder>::TransactionListItem(Holder * lst) : 
  list(lst)
{
  for(int i = list->size(); i > 0; )
    appendChild(new Item(list->pointerTo(--i)));
  connect(list->watchDog(), 
          SIGNAL(attributeChanged(const Watchdog *, const QString &)),
          SLOT(onAttributeChanged(const Watchdog *, const QString &)));

  connect(list->watchDog(), 
          SIGNAL(objectInserted(const Watchdog *, int, int)),
          SLOT(onObjectInserted(const Watchdog *, int, int)));

  connect(list->watchDog(), 
          SIGNAL(objectRemoved(const Watchdog *, int, int)),
          SLOT(onObjectRemoved(const Watchdog *, int, int)));

}

template <class Type, class Item, class Holder> 
void TransactionListItem<Type, Item, Holder>::effAttributeChanged(const Watchdog * wd, 
                                             const QString &name)
{
  if(name == "all") {
    // All the members were probably swapped, so we'll ensure the
    // transactions pointers are matching

    /// @todo assert that the source's size and the current number of
    /// children are the same
    int sz = list->size();
    for(int i = 0; i < sz; i++) {
      Item * it = dynamic_cast<Item *>(childAt(i));
      if(!it) {
        QTextStream o(stdout);
        o << "Null pointer " << endl;
        continue;               // But shouldn't happen
      }
      it->changeTransaction(list->pointerTo(sz - 1 - i));
    }
    
  }
}

template <class Type, class Item, class Holder> 
void TransactionListItem<Type, Item, Holder>::effObjectInserted(const Watchdog * wd, int at, int nb)
{
  int na = children.size() - at - 1;
  emit(rowsWillChange(this, at, nb));
  for(int i = nb - 1; i >= 0 ; i--)
    insertChild(na, new Item(list->pointerTo(at + nb - i - 1)));
  emit(rowsChanged(this));
}

template <class Type, class Item, class Holder> 
void TransactionListItem<Type, Item, Holder>::effObjectRemoved(const Watchdog * wd, int at, int nb)
{
}

template <class Type, class Item, class Holder> 
Account * TransactionListItem<Type, Item, Holder>::account() const
{
  if(list->size() > 0)
    return list->pointerTo(0)->getAccount();
  return NULL;
}


/// @todo This function can probably move one degree of abstraction
/// higher ??
template <class Type, class Item, class Holder> 
ModelItem * TransactionListItem<Type, Item, Holder>::findTransaction(const AtomicTransaction * tr)
{
  for(int i = 0; i < children.size(); i++) {
    ModelItem * item = children[i];
    Item * ti = dynamic_cast<Item *>(item);
    if(ti) {
      if(dynamic_cast<AtomicTransaction*>(ti->getTransaction()) == tr)
        return ti;
      else
        continue;
    }
  }
  return NULL;
}

/// @todo This function can probably move one degree of abstraction
/// higher. By this, I mean that all the objects that handle
/// transactions could descend from the same base class that has the
/// facility for finding transactions (and use it recursively when
/// applicable).
template <class Type, class Item, class Holder> 
ModelItem * TransactionListItem<Type, Item, Holder>::findTransaction(const Transaction * tr)
{
  for(int i = 0; i < children.size(); i++) {
    ModelItem * item = children[i];
    Item * ti = dynamic_cast<Item *>(item);
    if(ti) {
      if(dynamic_cast<Transaction*>(ti->getTransaction()) == tr)
        return ti;
      else
        continue;
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

void AccountModel::setList(TransactionList * t)
{
  this->transactions = t;
  setRoot(new TransactionListItem<Transaction, FullTransactionItem, TransactionList>(transactions));
}


void AccountModel::setList(TransactionPtrList * t)
{
  transactionsPtr = t;
  setRoot(new TransactionListItem<AtomicTransaction, LeafTransactionItem, TransactionPtrList>(transactionsPtr));
}

AccountModel::AccountModel(TransactionList * t) :
  OOModel(NULL), transactions(t), transactionsPtr(NULL)
{
  setList(t);
}

AccountModel::AccountModel(TransactionPtrList * t) :
  OOModel(NULL), transactions(NULL), transactionsPtr(t)
{
  setList(t);
}

Account * AccountModel::account() const
{

  BaseTransactionListItem * it = rootItem();
  if(it)
    return it->account();
  return NULL;
}

QModelIndex AccountModel::index(Transaction * transaction)
{
  ModelItem * it = rootItem()->findTransaction(transaction);
  if(it)
    return indexForItem(it);
  return QModelIndex();
}

BaseTransactionListItem * AccountModel::rootItem() const
{
  return dynamic_cast<BaseTransactionListItem *>(root);
}

/// @todo This show that, to some extent, FullTransactionItem and
/// LeafTransactionItem should share some code, probably through a
/// given base class ?
AtomicTransaction * AccountModel::indexedTransaction(QModelIndex index) const
{
  {
    FullTransactionItem * it = 
      dynamic_cast<FullTransactionItem *>(item(index));
    if(it)
      return it->getTransaction();
  }
  {
    LeafTransactionItem * it = 
      dynamic_cast<LeafTransactionItem *>(item(index));
    if(it)
      return it->getTransaction();
  }
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

