/*
    accountpage.cc: Implementation of AccountPage
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
#include <wallet.hh>
#include <transactionlistwidget.hh>

TransactionListWidget::TransactionListWidget(TransactionList *transactions,
					     QWidget * parent) :
  QWidget(parent), model(NULL) {
  setupFrame();
  showTransactions(transactions);
}

TransactionListWidget::TransactionListWidget(TransactionPtrList *transactions,
					     QWidget * parent) :
  QWidget(parent), model(NULL)  {
  setupFrame();
  showTransactions(transactions);
}

TransactionListWidget::TransactionListWidget(QWidget * parent) :
  QWidget(parent), model(NULL)  {
  setupFrame();
}

void TransactionListWidget::showTransactions(TransactionList *transactions)
{
  if(model)
    delete model;
  model = new AccountModel(transactions);
  setupTreeView();
}

void TransactionListWidget::showTransactions(TransactionPtrList *transactions)
{
  if(model)
    delete model;
  model = new AccountModel(transactions);
  setupTreeView();

  // By default, we hide the balance, as it usually doesnt make sense
  // for PtrLists
  hideBalance();
}


void TransactionListWidget::setupFrame()
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  view = new QTreeView(this);
  layout->addWidget(view);
}

void TransactionListWidget::setupTreeView()
{
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  view->setRootIsDecorated(false);

  if(wallet())
    view->setItemDelegateForColumn(AccountModel::CategoryColumn,
				   new AccountItemDelegate(wallet()));

  // Now fun
  view->setItemDelegateForColumn(AccountModel::LinksColumn,
				 new LinksItemDelegate);

  // Multiple selection
  view->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(view,SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(fireUpContextMenu(const QPoint &)));
  view->setContextMenuPolicy(Qt::CustomContextMenu);



  QModelIndex root = model->index(0,0,QModelIndex());
  /// \todo We need to intercept the signals from the model saying
  /// columns have been inserted to ensure all have their persistent
  /// editor opened.
  for(int i = 0; i < model->rowCount(root); i++)
    view->openPersistentEditor(root.child(i, AccountModel::LinksColumn));

  /// This is a must.
  view->setAlternatingRowColors(true);

  // We make sure the columns have the right size.
  for(int i = 0; i < AccountModel::LastColumn; i++)
    view->resizeColumnToContents(i);
}

TransactionListWidget::~TransactionListWidget()
{
  delete model;
}

void TransactionListWidget::showTransaction(Transaction * transaction)
{
  view->setCurrentIndex(model->index(transaction));
}

int TransactionListWidget::naturalWidth() const
{
  int size = 0;
  for(int i = 0; i < AccountModel::LastColumn; i++)
    if(! view->isColumnHidden(i))
      size += view->columnWidth(i);
  return size;
}

QSize TransactionListWidget::sizeHint() const
{
  /// \todo This is already significantly better than before, but I
  /// think the function gets called before the columns are resized,
  /// so it does not work too well...
  QSize size;
  size.setWidth(naturalWidth());
  // An estimate of the size ?
  size.setHeight(300);
  return size;
}

void TransactionListWidget::fireUpContextMenu(const QPoint & pos)
{
  // We fire up a neat menu to select categories, for instance.
  QMenu menu;
  QMenu * subMenu = new QMenu(tr("Set category"));
  QAction * clearAction = new QAction(tr("Clear"), this);
  clearAction->setData(QStringList() << "set-category" << "");
  subMenu->addAction(clearAction);
  subMenu->addSeparator();
  if(wallet())
    fillMenuWithCategoryHash(subMenu, &wallet()->categories);
  // TODO: do that for the "category submenu..."
  connect(subMenu, SIGNAL(triggered(QAction *)),
	  SLOT(contextMenuActionFired(QAction *)));
  menu.addMenu(subMenu);
  menu.exec(view->viewport()->mapToGlobal(pos));
}

void TransactionListWidget::fillMenuWithCategoryHash(QMenu * menu,
						     CategoryHash * ch)
{
  QStringList subCategories = ch->keys();
  subCategories.sort(); // Will be much easier to read !
  for(int i = 0; i < subCategories.count(); i++)
    fillMenuWithCategory(menu, &ch->operator[](subCategories[i]));
}



void TransactionListWidget::fillMenuWithCategory(QMenu * menu,
						 Category * category)
{
  QAction * a = new QAction(this);
  a->setData(QStringList() << "set-category" << category->fullName());
  if(category->subCategories.size()) {
    // Complex case:
    QMenu * subMenu = new QMenu(category->name);
    a->setText(tr("This category"));
    subMenu->addAction(a);
    subMenu->addSeparator();
    fillMenuWithCategoryHash(subMenu, &category->subCategories);
    menu->addMenu(subMenu);
  }
  else {
    a->setText(category->name);
    menu->addAction(a);
  }
}


TransactionPtrList TransactionListWidget::selectedTransactions() const
{
  TransactionPtrList list;
  QModelIndexList l = view->selectionModel()->selectedIndexes();
  for(int i = 0; i < l.size(); i++)
    list << model->indexedTransaction(l[i]);
  return list;
}

void TransactionListWidget::contextMenuActionFired(QAction * action)
{
  QStringList l = action->data().toStringList();
  QString what = l.takeFirst();
  TransactionPtrList selected = selectedTransactions();
  if(what == "set-category") {
    QString category;
    if(l.size() > 0) {
      category = l.first();
    }
    else {
      /// @todo implement a dialog box for new categories.
    }
    for(int i = 0; i < selected.count(); i++)
      selected[i]->setCategoryFromName(category);
  }
}

Wallet * TransactionListWidget::wallet() const
{
  if(model->account())
    return model->account()->wallet;
  return NULL;
}

void TransactionListWidget::showBalance() 
{
  view->showColumn(AccountModel::BalanceColumn);
}

void TransactionListWidget::hideBalance() 
{
  view->hideColumn(AccountModel::BalanceColumn);
}
