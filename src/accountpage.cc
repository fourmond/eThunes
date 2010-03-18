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

AccountPage::AccountPage(Account * ac) : account(ac)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  
  accountSummary = new QLabel("Account !");
  layout->addWidget(accountSummary);
  
  view = new QTreeView(this);
  layout->addWidget(view);

  /// \todo Won't do
  model = new AccountModel(account->transactions);
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  view->setRootIsDecorated(false);
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
