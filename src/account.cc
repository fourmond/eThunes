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
  // Here, we implicitly assume that the transaction list is sorted.
  t = t.sublist(*this);
  for(int i = 0; i < t.size(); i++)
    t[i].account = this;
  t.sortByDate();
  //  int dups = 
  t.removeDuplicates(transactions);
  // printf("%d duplicates were removed out of %d !!\n", dups, dups + t.size());
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
  ac->addAttribute("public-name", 
		   new SerializationItemScalar<QString>(&publicName, true));
  ac->addAttribute("bank-id", 
		   new SerializationItemScalar<QString>(&bankID));
  ac->addAttribute("branch-id", 
		   new SerializationItemScalar<QString>(&branchID));
  ac->addAttribute("type", 
		   new SerializationItemScalar<int>((int*)(&type)));

  // Now, we try the list stuff...
  ac->addAttribute("transaction", 
		   new SerializationQList<Transaction>(&transactions));
  return ac;
}

void Account::sanitizeAccount()
{
  transactions.sanitizeList(this);
  transactions.computeBalance();
}

void Account::clearContents()
{
  transactions.clear();
}

QList<Transaction *> Account::categoryTransactions(const Category * category, 
						   bool parents)
{
  QList<Transaction *> found;
  for(int i = 0; i < transactions.size(); i++) {
    Transaction * t = &transactions[i];
    if(t->category == category || (parents && t->category 
				   && t->category->isChildOf(category)))
      found.push_back(t);
  }
  return found;
}
