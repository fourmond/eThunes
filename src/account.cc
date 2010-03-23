/*
    account.cc: The Account class
    Copyright 2008, 2010 by Vincent Fourmond

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

QString Account::name()
{
  if(publicName.isEmpty())
    return accountID();
  return publicName;
}


int Account::importTransactions(TransactionList t)
{
  /// \todo Here, add checks for duplicates. Or somehow before ?
  t = t.sublist(*this);
  for(int i = 0; i < t.size(); i++)
    t[i].account = this;
  transactions.append(t);
  // Now, we sort and update the balance
  transactions.sortByDate();
  transactions.computeBalance();
  
  return t.size();
}

SerializationAccessor * Account::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("number", 
		   new SerializationItemScalar<QString>(&accountNumber));

  // Now, we try the list stuff...
  ac->addAttribute("transaction", 
		   new SerializationQList<Transaction>(&transactions));
  ac->
    addCallback(new SerializationMethodCallback<Account>(this, 
							 &Account::sanitizeAccount));
  return ac;
}

void Account::sanitizeAccount()
{
  transactions.sanitizeList(this);
  transactions.computeBalance();
}
