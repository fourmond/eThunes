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
#include <wallet.hh>		// For filtering

Account::Account() : wallet(NULL) 
{
  watchChild(&transactions, "transactions");
}

QString Account::name() const
{
  if(publicName.isEmpty())
    return accountID();
  return publicName;
}


int Account::importTransactions(TransactionList t, Wallet * filters)
{
  // Here, we implicitly assume that the transaction list is sorted.
  t = t.sublist(*this);
  for(int i = 0; i < t.size(); i++)
    t[i].account = this;
  t.sortByDate();
  //  int dups =
  t.removeDuplicates(transactions);
  if(filters)
    filters->runFilters(&t);
  /// \todo This should be logged !
  // printf("%d duplicates were removed out of %d !!\n", dups, dups + t.size());

  // We now mark imported transactions as recent.
  t.markRecent();

  transactions.append(t);
  // Now, we sort and update the balance
  transactions.sortByDate();
  transactions.computeBalance();

  return t.size();
}

SerializationAccessor * Account::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("number",&accountNumber);
  ac->addScalarAttribute("public-name", &publicName);
  ac->addScalarAttribute("bank-id", &bankID);
  ac->addScalarAttribute("branch-id", &branchID);
  ac->addScalarAttribute("type",(int*)(&type));
  ac->addListAttribute("transaction", &transactions);
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

TransactionPtrList Account::allTransactions()
{
  /// @todo This should be rewritten using iterators !
  TransactionPtrList ret;
  for(int i = 0; i < transactions.size(); i++)
    ret.append(transactions[i].allSubTransactions());
  return ret;
}


TransactionPtrList Account::categoryTransactions(const Category * category,
                                                 bool parents)
{
  TransactionPtrList found;
  /// @todo Rewrite using iterators when available !
  TransactionPtrList all = allTransactions();
  for(int i = 0; i < all.size(); i++) {
    AtomicTransaction * t = all[i];
    if(t->getCategory() == category || 
       (parents && t->getCategory() && 
        t->getCategory()->isChildOf(category))
       )
      found.append(t);
  }
  return found;
}

TransactionPtrList Account::taggedTransactions(const Tag * tag)
{
  TransactionPtrList lst;
  /// @todo Rewrite using iterators when available !
  TransactionPtrList all = allTransactions();
  for(int i = 0; i < all.size(); i++) {
    AtomicTransaction * t = all[i];
    if(t->hasTag(tag))
      lst.append(t);
  }
  return lst;
}

int Account::firstMonthID() const
{
  if(transactions.size() > 0)
    return transactions[0].monthID();
  return -1;
}



TransactionPtrList Account::checks()
{
  TransactionPtrList t;
  for(int i = 0; i < transactions.size(); i++)
    if(! transactions[i].getCheckNumber().isEmpty())
      t << &transactions[i];
  QList<AtomicTransaction*> & lst = t.rawData();
  qSort(lst.begin(), lst.end(), Transaction::compareCheckNumbers);
  return t;
}

TransactionPtrList Account::recentTransactions()
{
  TransactionPtrList t;
  for(int i = 0; i < transactions.size(); i++)
    if(transactions[i].isRecent())
      t << &transactions[i];
  return t;
}

/// @todo far from optimal, but working.
TransactionPtrList Account::monthlyTransactions(int monthID)
{
  TransactionPtrList t;
  for(int i = 0; i < transactions.size(); i++)
    if(transactions[i].monthID() == monthID)
      t << &transactions[i];
  return t;
}

Transaction * Account::namedTransaction(const QString & name)
{
  /// \todo This will eventually be *very* slow...
  for(int i = 0; i < transactions.size(); i++)
    if(transactions[i].transactionID() == name)
      return &transactions[i];
  return NULL;
}

int Account::balance(const QDate & date) const
{

  QList<QDate> mirror;          /// @todo bad, but, well...  Maybe
                                /// with a cache it would be nice ?
  for(int i = 0; i < transactions.size(); i++)
    mirror << transactions[i].getDate();

  int which =  qUpperBound(mirror, date) - mirror.begin();
  if(!which)
    return 0;
  else {
    which--;
    return transactions[which].getBalance();
  }
}
