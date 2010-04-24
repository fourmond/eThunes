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
#include <accountpage.hh>
#include <transactionlistdialog.hh>


QHash<Account *, AccountPage *> AccountPage::accountPages;

AccountPage::AccountPage(Account * ac) : account(ac)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  
  accountSummary = new QLabel();
  layout->addWidget(accountSummary);
  updateAccountSummary();
  
  view = new QTreeView(this);
  layout->addWidget(view);

  /// \todo Won't do
  model = new AccountModel(&(account->transactions));
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  view->setRootIsDecorated(false);

  view->setItemDelegateForColumn(AccountModel::CategoryColumn,
				 new AccountItemDelegate(account->wallet));
  view->setAlternatingRowColors(true);
  
  for(int i = 0; i < AccountModel::LastColumn; i++)
    view->resizeColumnToContents(i);
  connect(accountSummary, SIGNAL(linkActivated(const QString &)), 
	  SLOT(handleLinks(const QString &)));
}

AccountPage::~AccountPage()
{
  /// \todo Here, delete quite a few things.
}

QString AccountPage::pageTitle()
{
  if(account)
    return account->name();
  return tr("??");
}

void AccountPage::updateAccountSummary()
{
  /// \todo there is a great deal to change here.
  accountSummary->setText(tr("<strong>Account: </strong> %1 <a href='#name'>(change name)</a><br>"
			     "<strong>Balance: </strong> %2\t"
			     "<a href='#checks'>(see checks)</a><br>").
			  arg(account->name()).
			  arg(account->balance() * 0.01 ));
}

AccountPage * AccountPage::getAccountPage(Account * account)
{
  if(! accountPages.contains(account))
    accountPages[account] = new AccountPage(account);
  return accountPages[account];
}

void AccountPage::renameAccount()
{
  QString newName = QInputDialog::getText(this, tr("New name"),
					  tr("New name for the account"));
  if(newName.isEmpty())
    return;
  account->publicName = newName;
  updateAccountSummary();
}

void AccountPage::handleLinks(const QString & url)
{
  if(url == "#name")
    renameAccount();
  else if(url == "#checks")
    displayChecks();
}
void AccountPage::displayChecks()
{
  TransactionListDialog * checks = new TransactionListDialog();
  checks->displayChecks(account);
  checks->show();
}
