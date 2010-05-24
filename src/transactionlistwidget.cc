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

  view->setItemDelegateForColumn(AccountModel::CategoryColumn,
				 new AccountItemDelegate(model->
							 account()->wallet));

  // Now fun
  view->setItemDelegateForColumn(AccountModel::LinksColumn,
				 new LinksItemDelegate);
  // This ain't gonna work, but it's worth a try ;-)...
  QModelIndex root = model->index(0,0,QModelIndex());
  /// \todo We need to intercept the signals from the model saying
  /// columns have been inserted to ensure all have their persistent
  /// editor opened.
  for(int i = 0; i < model->rowCount(root); i++)
    view->openPersistentEditor(root.child(i, AccountModel::LinksColumn));


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
