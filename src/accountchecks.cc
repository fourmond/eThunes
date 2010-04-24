/*
    accountchecks.cc: displaying cheques
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
#include <accountchecks.hh>
#include <accountmodel.hh>

AccountChecks::AccountChecks(Account * c) :
  account(c)
{
  // First, a VBox:
  QVBoxLayout * l1 = new QVBoxLayout(this);
  l1->addWidget(new QLabel(tr("Account: %1").arg(account->name())));

  list = account->checks();

  AccountModel * model = new AccountModel(&list);

  QTreeView * v = new QTreeView();
  v->setModel(model);
  v->setRootIndex(model->index(0,0));
  v->setRootIsDecorated(false);
  v->setItemDelegateForColumn(AccountModel::CategoryColumn,
			      new AccountItemDelegate(account->wallet));
  v->setAlternatingRowColors(true);
  for(int i = 0; i < AccountModel::LastColumn; i++)
    v->resizeColumnToContents(i);

  l1->addWidget(v);

  QPushButton * bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(bt);
}

