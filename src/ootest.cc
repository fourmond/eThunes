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

class TransactionItem : public LeafModelItem {

  Transaction * transaction;
public:
  TransactionItem(Transaction * t) : transaction(t) {
  };


  virtual int columnCount() const {
    return AccountModel::LastColumn;
  };

  virtual QVariant data(int column, int role) const {
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
};


class TransactionListItem : public FixedChildrenModelItem {

  TransactionList * transactions;

public:
  TransactionListItem(TransactionList * trs) : transactions(trs) {
    for(int i = transactions->size(); i > 0; )
      appendChild(new TransactionItem(&(transactions->operator[](--i))));
  };

  virtual QVariant data(int column, int role) const {
    return QVariant();
  };
  
};


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
