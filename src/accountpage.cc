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
#include <htlabel.hh>
#include <httarget-templates.hh>


QHash<Account *, AccountPage *> AccountPage::accountPages;

AccountPage::AccountPage(Account * ac) : account(ac)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  accountSummary = new HTLabel();
  layout->addWidget(accountSummary);
  updateAccountSummary();

  view = new TransactionListWidget(&(account->transactions),this);
  layout->addWidget(view);
}

AccountPage::~AccountPage()
{
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
  QString str = tr("<strong>Account: </strong> %1 %2<br>"
                   "<strong>Balance: </strong> %3 ").
    arg(account->name()).
    arg(HTTarget::linkToMember("(rename)", this, 
                               &AccountPage::renameAccount)).
    arg(account->balance() * 0.01 );
  
  str += HTTarget::linkToFunction("(see checks)",
                                  &TransactionListDialog::showChecks,
                                  account);
  accountSummary->setText(str);
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

void AccountPage::showTransaction(Transaction * transaction)
{
  view->showTransaction(transaction);
}
