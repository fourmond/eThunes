/*
    transactionlistdialog.cc: displaying list of Transactions
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
#include <transactionlistdialog.hh>
#include <wallet.hh>

TransactionListDialog::TransactionListDialog()
{
  // First, a VBox:
  QVBoxLayout * l1 = new QVBoxLayout(this);
  topLabel = new QLabel();
  l1->addWidget(topLabel);

  view = new TransactionListWidget();
  l1->addWidget(view);

  QPushButton * bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(bt);

  // Force the deletion on close
  setAttribute(Qt::WA_DeleteOnClose);
}

TransactionListDialog::~TransactionListDialog()
{
}

void TransactionListDialog::displayList(const TransactionPtrList & l,
					const QString & label)
{
  list = l;
  view->showTransactions(&list);
  topLabel->setText(label);
}

void TransactionListDialog::displayChecks(Account * account)
{
  displayList(account->checks(),
	      tr("Account: %1").arg(account->name()));
}

void TransactionListDialog::displayCategory(Category * category,
					    Wallet * wallet)
{
  displayList(wallet->categoryTransactions(category),
	      tr("Category: %1").arg(category->fullName()));
}
