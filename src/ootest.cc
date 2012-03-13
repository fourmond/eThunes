/*
  ootest.cc: test widget for OOModel
  Copyright 2012 by Vincent Fourmond

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
#include <ootest.hh>
#include <oomodel.hh>
#include <modelitems.hh>

#include <widgetwrapperdialog.hh>

#include <accountmodel.hh>

class RootItem : public FixedChildrenModelItem {
  
public:
  RootItem(int rem = 1) {
    appendChild(new TextModelItem("biniou"));
    appendChild(new TextModelItem("bidule"));
    appendChild(new TextModelItem(QString("truc %1").arg(rem)));
    appendChild(new TextModelItem(QStringList() << "machin" << "stuff"));
    if(rem > 0)
      appendChild(new RootItem(rem - 1));
  };

  virtual QVariant data(int column, int role) const {
    if(role == Qt::DisplayRole && column == 0)
      return "Root";
    return QVariant();
  };

  void makeNew(const QString &s) {
    insertChild(1, new TextModelItem(s));
  };

  void modifyFirst() {
    static int nb = 0;
    TextModelItem * it = static_cast<TextModelItem*>(children.first());
    it->setText(QString("Modification #%1").arg(++nb));
  };

  virtual QVariant headerData(int column, Qt::Orientation /*orientation*/, 
                              int role) const {
    if(role != Qt::DisplayRole)
      return QVariant();
    return QString("column %1").arg(column);
  };

};

//////////////////////////////////////////////////////////////////////

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
  transactions(trs) 
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

QVariant TransactionListItem::data(int column, int role) const
{
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
  // hmmm, we are working in reverse !
  QTextStream o(stdout);
  o << "Changing transactions: " << at << " -- " << nb << endl;
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



OOTest::OOTest(TransactionList * transactions)
{
  model = new OOModel(new TransactionListItem(transactions));
  QVBoxLayout * layout = new QVBoxLayout(this);
  
  view = new QTreeView();
  view->setModel(model);
  layout->addWidget(view);
  view->setRootIndex(model->rootIndex());
  view->setRootIsDecorated(false);

}

void OOTest::test(TransactionList * transactions)
{
  WidgetWrapperDialog * dlg = 
    new WidgetWrapperDialog(new OOTest(transactions), 
                            tr("test !"));
  dlg->show();
}

OOTest::~OOTest()
{
  delete model;
}
