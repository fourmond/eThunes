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
#include <ootest.hh>

#include <accountmodel.hh>
#include <widgetwrapperdialog.hh>

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
  if(! model)
    model = new AccountModel(transactions);
  else
    model->setList(transactions);
  setupTreeView(true);
}

void TransactionListWidget::showTransactions(TransactionPtrList *transactions)
{
  if(! model)
    model = new AccountModel(transactions);
  else
    model->setList(transactions);
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

void TransactionListWidget::setupTreeView(bool decorate)
{
  view->setModel(model);

  QModelIndex root = model->rootIndex();
  
  view->setRootIndex(root);
  view->setRootIsDecorated(decorate);

  if(wallet())
    view->setItemDelegateForColumn(AccountModel::CategoryColumn,
				   new AccountItemDelegate(wallet()));

  // Now fun
  view->setItemDelegateForColumn(AccountModel::LinksColumn,
				 new LinksItemDelegate);

  // Multiple selection
  view->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(fireUpContextMenu(const QPoint &)));
  view->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(view, SIGNAL(expanded(const QModelIndex &)),
	  SLOT(onItemExpanded(const QModelIndex &)));

  connect(model, 
          SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          SLOT(ensureEditorsOn(const QModelIndex &, const QModelIndex &)));




  /// This is a must.
  view->setAlternatingRowColors(true);

  onItemExpanded(root);
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
  QModelIndex idx = model->index(transaction);
  if(idx.isValid())
    view->setCurrentIndex(idx);
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
  /// @todo This function should disappear to be handled by the
  /// objects themselves.
  ///
  /// However, handling a full selection won't come in too easy

  // We fire up a neat menu to select categories, for instance.
  QMenu menu;

  QMenu * subMenu = new QMenu(tr("Set category"));
  QAction * action = new QAction(tr("Clear"), this);
  action->setData(QStringList() << "set-category" << "");
  subMenu->addAction(action);
  subMenu->addSeparator();
  if(wallet())
    fillMenuWithCategoryHash(subMenu, &wallet()->categories);
  menu.addMenu(subMenu);

  subMenu = new QMenu(tr("Clear tag"));
  if(wallet()) {
    fillMenuWithTags(subMenu, &wallet()->tags, "clear-tag");
  }
  menu.addMenu(subMenu);

  subMenu = new QMenu(tr("Add tag"));
  if(wallet()) {
    fillMenuWithTags(subMenu, &wallet()->tags, "add-tag");
  }
  menu.addMenu(subMenu);

  if(true) { /// @todo add a readonly attribute ?
    /// @todo add only for real transactions ?
    QAction * a = new QAction(tr("Add subtransaction"), this);
    a->setData(QStringList() << "add-sub");
    menu.addAction(a);
  }

  
  menu.addSeparator();
  action = new QAction(tr("Statistics"), this);
  action->setData(QStringList() << "stats");
  menu.addAction(action);
  

  connect(&menu, SIGNAL(triggered(QAction *)),
	  SLOT(contextMenuActionFired(QAction *)));
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

void TransactionListWidget::fillMenuWithTags(QMenu * menu, TagHash * tags, 
					     const QString &action)
{
  QStringList t = tags->tagNames();
  t.sort();
  for(int i = 0; i < t.count(); i++) {
    QAction * a = new QAction(t[i], this);
    a->setData(QStringList() << action << t[i]);
    menu->addAction(a);
  }
  
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
  QSet<AtomicTransaction *> dup;
  for(int i = 0; i < l.size(); i++) {
    AtomicTransaction * t = model->indexedTransaction(l[i]);
    if(t && !(dup.contains(t))) {
      list << t;
      dup.insert(t);
    }
  }
  return list;
}

AtomicTransaction * TransactionListWidget::currentTransaction() const
{
  return model->indexedTransaction(view->currentIndex());
}

/// @todo This and the corresponding menu-populating function must
/// somehow be reimplemented by the AccountModel.
///
/// Maybe there should be ways to customize the presence of things -
/// ie have different modes (a read-only mode, a read-write, and so
/// on...). This could be implemented thus:
/// @li having a ModelItem function that returns annotated connected
/// actions
/// @li having the AccountModel that takes care of building a menu
/// with that, possibly filtering out some of them ?
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
    /// @todo handle the signaling here ?
  } else if(what == "clear-tag") {
    Tag * t = wallet()->namedTag(l.first());
    if(t) {
      for(int i = 0; i < selected.count(); i++)
	selected[i]->clearTag(t);
      
    }
  } else if(what == "add-tag") { 
    Tag * t = wallet()->namedTag(l.first());
    if(t) {
      for(int i = 0; i < selected.count(); i++)
	selected[i]->setTag(t);
    }
  } else if(what == "add-sub") { 
    // Add a subtransaction to the current transaction
    Transaction * t = dynamic_cast<Transaction*>(currentTransaction());
    if(! t)
      return;                   // Must be a real transaction !
    int amount = 
      QInputDialog::getInt(this, 
                           tr("Create subtransaction"),
                           tr("Enter subtransaction amount"));
    if(! amount)
      return;
    t->subTransactions.append(AtomicTransaction(amount, t));
  } else if(what == "stats") { 
    TransactionListStatistics stats = selected.statistics();
    QString statsString = 
      tr("<b>Stats: </b><br/>\n"
         "Total: %1<br/>\n"
         "Number of transactions: %2").
      arg(Transaction::formatAmount(stats.totalAmount)).
      arg(stats.number);
    WidgetWrapperDialog * dlg = 
      new WidgetWrapperDialog(new QLabel(statsString));
    dlg->show();
  } else {
    /// @todo Log !
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

void TransactionListWidget::onItemExpanded(const QModelIndex & idx)
{
  if(idx.isValid()) {
    // Make sure the persistent editors are opened
    int rc = idx.model()->rowCount(idx);
    ensureEditorsOn(idx.child(0, AccountModel::LinksColumn),
                    idx.child(rc-1, AccountModel::LinksColumn));
  }
}

void TransactionListWidget::ensureEditorsOn(const QModelIndex & tl, 
                                            const QModelIndex & bl)
{
  if(! (tl.isValid() && bl.isValid()))
    return;                     // Doesn't make sense.
  QModelIndex idx = tl.sibling(tl.row(), AccountModel::LinksColumn);
  while(idx.isValid() && (idx < bl || idx == bl)) {
    QString s = idx.data(Qt::EditRole).toString();
    if(! s.isEmpty())
      view->openPersistentEditor(idx);
    idx = idx.sibling(idx.row() + 1, AccountModel::LinksColumn);
  }
}
