/*
    transactionlists.cc: implementation of the Transaction lists class
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
#include <transaction.hh>
#include <account.hh>
#include <wallet.hh>

BasicStatistics::BasicStatistics() :
  number(0), totalAmount(0)
{
}

void BasicStatistics::addTransaction(const Transaction * t)
{
  number += 1;
  totalAmount += t->amount;
}

TransactionListStatistics::TransactionListStatistics() 
{
}

void TransactionListStatistics::addTransaction(const Transaction * t)
{
  BasicStatistics::addTransaction(t);
  monthlyStats[t->monthID()].addTransaction(t);
}

int TransactionListStatistics::monthlyAverageAmount()
{
  int nb = 0;
  int total = 0;
  int thisMonthID = Transaction::thisMonthID();
  QHash<int, BasicStatistics>::const_iterator i = monthlyStats.constBegin();
  while (i != monthlyStats.constEnd()) {
    if(i.key() != thisMonthID) {
      nb += 1;
      total += i.value().totalAmount;
    }
    i++;
  }
  if(nb)
    return total/nb;
  return 0;
}

TransactionListStatistics TransactionPtrList::statistics() const
{
  TransactionListStatistics stats;
  for(int i = 0; i < size(); i++)
    stats.addTransaction(value(i));
  return stats;
}

TransactionList TransactionList::sublist(const Account &ac)
{
  TransactionList retval;
  for(int i = 0; i < size(); i++)
    if(at(i).account && at(i).account->isSameAccount(ac))
      retval << at(i);
  return retval;
}

void TransactionList::sortByDate()
{
  qSort(*this);
}

void TransactionList::computeBalance(int balance)
{
  /// \todo Maybe there should be a way to check the balance, through
  /// a checkBalance function ?
  for(int i = 0; i< size(); i++) {
    Transaction & t = operator[](i); // Rather inelegant, I find...
    balance += t.amount;
    t.balance = balance;
    t.balanceMeaningful = true;
  }
}

int TransactionList::removeDuplicates(const TransactionList & other)
{
  // We first make sure that we are starting around the same dates.
  // We remind here that the list must be sorted !
  int i = 0, j = 0;
  int retval = 0;

  while(i < size() && j < other.size()) {
    if(at(i) == other.at(j)) {
      removeAt(i);
      retval++;
    }
    else {
      if(other.at(j) < at(i))
	j++;
      else
	i++; 
    }
  }
  return retval;
}

void TransactionList::sanitizeList(Account * ac)
{
  for(int i = 0; i < size(); i++) {
    if(at(i).account) {
      if(at(i).account == ac || at(i).account->isSameAccount(*ac))
	operator[](i).account = ac;
      else {
	removeAt(i); i--;
      }
    }
    else {
      operator[](i).account = ac;
    }
  }
}

TransactionPtrList TransactionList::toPtrList()
{
  TransactionPtrList list;
  for(int i = 0; i < size(); i++)
    list << &(operator[](i));
  return list;
}
