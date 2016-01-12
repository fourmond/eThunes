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
					const QString & label, bool multi)
{
  list = l;
  view->showTransactions(&list);
  if(multi)
    view->showAccountName();
  topLabel->setText(label);
}

void TransactionListDialog::displayChecks(Account * account)
{
  displayList(account->checks(),
	      tr("Checks for account %1").arg(account->name()));
}

void TransactionListDialog::displayCategory(Category * category,
					    Wallet * wallet)
{
  displayList(wallet->categoryTransactions(category),
	      tr("Category: %1").arg(category->fullName()));
}

void TransactionListDialog::displayTag(Tag * tag,
                                       Wallet * wallet)
{
  displayList(wallet->taggedTransactions(tag),
	      tr("Tag: %1").arg(tag->name));
}

void TransactionListDialog::displayMonthlyTransactions(Account * account, 
                                                       int monthID, 
                                                       int number)
{
  QList<Account *> acs;
  acs << account;
  displayMonthlyTransactions(acs, monthID, number);
}

void TransactionListDialog::displayMonthlyTransactions(const QList<Account *> accounts,
                                                       int monthID, 
                                                       int number)
{
  QDate d = Transaction::dateFromID(monthID);
  
  TransactionPtrList list;

  QStringList names;
  for(int j = 0; j < accounts.size(); j++) {
    Account * account = accounts[j];
    names << account->name();
    for(int i = 0; i < number; i++)
      list.append(account->monthlyTransactions(monthID + i));
  }
  list.sortByDate();

  QString label;
  if(number == 1)
    label = d.toString("MMMM yyyy");
  else {
    label = tr("%1 to %2").arg(d.toString("MMMM yyyy")).
      arg(d.addMonths(number-1).toString("MMMM yyyy"));
  }

  QString lbl;
  if(accounts.size() > 1)
    lbl = tr("Accounts: %1 -- %2").arg(names.join(", ")).arg(label);
  else
    lbl = tr("Account: %1 -- %2").arg(names.first()).arg(label);
  
  displayList(list, lbl, accounts.size() > 0);
  view->showBalance();
              
}


void TransactionListDialog::
displayMonthlyCategoryTransactions(Category * category,
                                   Account * account, 
                                   int monthID, int number)
{
  QList<Account *> acs;
  acs << account;
  displayMonthlyCategoryTransactions(category, acs, monthID, number);
}

void TransactionListDialog::
displayMonthlyCategoryTransactions(Category * category,
                                   const QList<Account *>& accounts, 
                                   int monthID, int number)
{
  QDate d = Transaction::dateFromID(monthID);
  TransactionPtrList l;
  for(int i = 0; i < accounts.size(); i++) {
    Account * account = accounts[i];
    l.append(account->categoryTransactions(category));
  }
  TransactionPtrList l2;
  for(int i = 0; i < l.size(); i++)
    if(l[i]->monthID() >= monthID && l[i]->monthID() < monthID + number)
      l2.append(l[i]);
  QString label;
  if(number == 1)
    label = d.toString("MMMM yyyy");
  else {
    label = tr("%1 to %2").arg(d.toString("MMMM yyyy")).
      arg(d.addMonths(number-1).toString("MMMM yyyy"));
  }

  l2.sortByDate();

  displayList(l2,
	      tr("Category: %1 -- %2").
              arg(category ? category->name : "(uncategorized)").
              arg(label), accounts.size() > 0);
}

void TransactionListDialog::showChecks(Account * account)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayChecks(account);
  dlg->show();
}

void TransactionListDialog::showCategory(Category * category, Wallet * wallet)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayCategory(category, wallet);
  dlg->show();
}

void TransactionListDialog::showMonthlyTransactions(Account * account, 
                                                    int monthID)
{
  showMonthlyTransactions(account, monthID, 1);
}

void TransactionListDialog::showMonthlyTransactions(Account * account, 
                                                    int monthID, int number)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayMonthlyTransactions(account, monthID, number);
  dlg->show();
}

void TransactionListDialog::showMonthlyTransactions(const QList<Account *> accounts, 
                                                    int monthID, int number)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayMonthlyTransactions(accounts, monthID, number);
  dlg->show();
}

void TransactionListDialog::showMonthlyCategoryTransactions(Category * category,
                                                            Account * account, 
                                                            int monthID)
{
  showMonthlyCategoryTransactions(category, account, monthID, 1);
}

void TransactionListDialog::showMonthlyCategoryTransactions(Category * category,
                                                            Account * account, 
                                                            int monthID, 
                                                            int number)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayMonthlyCategoryTransactions(category, account, monthID, number);
  dlg->show();
}

void TransactionListDialog::showMonthlyCategoryTransactions(Category * category,
                                                            QList<Account *> accounts, 
                                                            int monthID, 
                                                            int number)
{
  TransactionListDialog * dlg = new TransactionListDialog();
  dlg->displayMonthlyCategoryTransactions(category, accounts, monthID, number);
  dlg->show();
}
