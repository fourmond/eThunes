/*
    transaction.cc: implementation of the Transaction class
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

void Transaction::dump(QIODevice * dev)
{
  QTextStream stream(dev);
  dump(stream);
}

void Transaction::dump(QTextStream & stream)
{
  stream << "Transaction: \"" << name << "\" of "
	 << amount/100.0 << " (" 
	 << date.toString("dd/MM/yyyy") << ")" << endl;
  if(account)
    stream << " -> account: " << account->name() << endl;
  else
    stream << " -> no valid account" << endl;
  if(! memo.isEmpty())
    stream << " -> memo: \"" << memo << "\"" << endl;
  if(checkNumber)
    stream << " -> check number: " << checkNumber << endl;
}

Transaction::Transaction() :
  amount(0),
  checkNumber(0),
  account(NULL),
  locked(1),
  balanceMeaningful(false)
{
}

bool Transaction::operator<(const Transaction & t) const
{ 
  if(date != t.date)
    return date < t.date;
  else if(name != t.name)
    return name < t.name;
  else if(amount != t.amount)
    return amount < t.amount;
  /// \todo maybe look at other ones ?
  return true;			// By default, but they really should
				// be equal by that time!
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
    Transaction & t = this->operator[](i); // Rather inelegant, I
					   // find...
    balance += t.amount;
    t.balance = balance;
    t.balanceMeaningful = true;
  }
}

int TransactionList::removeDuplicates(const TransactionList & other)
{
  // We first make sure that we are starting around the same dates.
  int i = 0, j = 0;
  if(first().date < other.first().date) {
    while(i <  size() && at(i).date < other.first().date)
      i++;
    if(i == size())
      return 0;			// No duplicates for sure
  }
  // To be continued...

}
