/*
    linkable.cc: implementation of the Linkable superclass
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
#include <accountgroup.hh>
#include <wallet.hh>

SerializationAccessor * AccountGroup::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name);
  ac->addScalarListAttribute("account", &accountStringList, "id");
  return ac;
}

void AccountGroup::prepareSerializationWrite()
{
  accountStringList.clear();
  for(int i = 0; i < accounts.size(); i++)
    accountStringList.append(accounts[i]->accountID());
}


void AccountGroup::finalizePointers(Wallet * wallet)
{
  accounts.clear();
  for(int i = 0; i < accountStringList.size(); i++)
    accounts.append(wallet->namedAccount(accountStringList[i]));
  attributeChanged("accounts");
}

int AccountGroup::balance() const
{
  int balance = 0;
  for(int i = 0; i < accounts.size(); i++)
    balance += accounts[i]->balance();
  return balance;
}
