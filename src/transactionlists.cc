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

#include <logstream.hh>

BasicStatistics::BasicStatistics() :
  number(0), numberCredit(0), numberDebit(0), 
  totalAmount(0), totalCredit(0),
  totalDebit(0), firstMonthID(-1)
{
}

void BasicStatistics::addTransaction(const Transaction * t)
{
  number += 1;
  totalAmount += t->getAmount();
  if(t->account && (firstMonthID < 0 ||
		    t->account->firstMonthID() < firstMonthID))
    firstMonthID = t->account->firstMonthID();
  if(t->getAmount() < 0) {
    totalDebit += t->getAmount();
    numberDebit ++;
  }
  else {
    totalCredit += t->getAmount();
    numberCredit ++;
  }
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
  int total = 0;
  const int thisMonthID = Transaction::thisMonthID();
  QHash<int, BasicStatistics>::const_iterator i = monthlyStats.constBegin();
  while (i != monthlyStats.constEnd()) {
    if(i.key() != thisMonthID)
      total += i.value().totalAmount;
    i++;
  }
  if(firstMonthID < thisMonthID)
    return total/(thisMonthID - firstMonthID);
  return 0;
}

TransactionListStatistics TransactionPtrList::statistics() const
{
  TransactionListStatistics stats;
  for(int i = 0; i < size(); i++)
    stats.addTransaction(value(i));
  return stats;
}

TransactionList TransactionList::sublist(const Account &ac) const
{
  TransactionList retval;
  for(int i = 0; i < size(); i++)
    if(operator[](i).account && operator[](i).account->isSameAccount(ac))
      retval << operator[](i);
  return retval;
}


/// Here begins the code for pointer-safe sorting:
template <typename T> class MyQListIterator {
public:
  QList<T> * list;

  int idx;

  /// The exact same thing as QListIterator, but under another name
  /// and with different semantics.
  class PointedTo {
  public:
    QList<T> * list;

    int idx;

    const T & value() const {
      return (*list)[idx];
    };

    PointedTo(const MyQListIterator & i) : list(i.list), idx(i.idx) {;};
    
    bool operator <(const PointedTo & b) const {
      return value() < b.value();
    };

  };
  
  MyQListIterator(QList<T> * l, int i) : list(l), idx(i) {;};

  PointedTo operator*() { return PointedTo(*this);};

  MyQListIterator operator+(int i) const { 
    return MyQListIterator(list, idx+i);
  };

  MyQListIterator operator-(int i) const { 
    return MyQListIterator(list, idx-i);
  };

  bool operator<(const MyQListIterator & b) const {
    return idx < b.idx;
  };

  bool operator!=(const MyQListIterator & b) const {
    return idx != b.idx;
  };

  MyQListIterator& operator++() {
    idx++;
    return *this;
  };

  MyQListIterator& operator--() {
    idx--;
    return *this;
  };

  operator int() const {
    return idx;
  };
  
};

// Funny, this should be explicit and not a template, else g++ is
// unable to pick it.
inline void qSwap(MyQListIterator<Transaction>::PointedTo a, 
                  MyQListIterator<Transaction>::PointedTo b) {
  a.list->swap(a.idx, b.idx);
}


void TransactionList::sortByDate()
{
  qSort(MyQListIterator<Transaction>(&rawData(), 0),
        MyQListIterator<Transaction>(&rawData(), size()));
}

void TransactionList::computeBalance(int balance)
{
  /// \todo Maybe there should be a way to check the balance, through
  /// a checkBalance function ?
  for(int i = 0; i< size(); i++) {
    Transaction & t = operator[](i); // Rather inelegant, I find...
    balance += t.getAmount();
    t.setBalance(balance);
    // t.balanceMeaningful = true;
    // for now useless ?
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

TransactionPtrList TransactionList::transactionsWithinRange(const QDate & before, const QDate & after)

{
  TransactionPtrList list;
  if(before > after)
    return list;
  for(int i = 0; i < size(); i++) {
    if(at(i).getDate() >= before && 
       at(i).getDate() <= after)
      list << &(operator[](i));
    if(at(i).getDate() > after)
      break;
  }
  return list;
}

/// Small private helper class.
class IndexedDate {
public:
  QDate d;
  int i;

  IndexedDate() {;};

  IndexedDate(const QDate & da, int id): d(da), i(id) {
    ;
  };

  bool operator<(const IndexedDate & b) const {
    return d < b.d;
  };
};

/// Private class serving as a Key for finding transactions.
class HashKey {
public:
  QString memo;
  int amount;
  HashKey() {;};
  HashKey(const Transaction * t, bool permissive) {
    memo = (permissive ? "": t->getMemo());
    amount = abs(t->getAmount());
  };
  bool operator==(const HashKey & other) const {
    return (memo == other.memo) && 
      (amount == other.amount);
  };
};

uint qHash(const HashKey & k) {
  return qHash(k.memo) ^ k.amount;
}

QList<Link *> TransactionPtrList::findInternalMoves(QList<TransactionPtrList> lists, bool permissive)
{
  QList<Link *> retval;
  // We'll proceed by browsing through all the transactions date by
  // date.
  QList<QListIterator<Transaction *> > iterators;

  QList<IndexedDate> dates;
  QMultiHash<HashKey, Transaction *> transactions;
  QList<HashKey> tkeys;
  QList<Transaction *> tvalues;
  LogStream err(Log::Error);

  // Initialize the iterators.
  for(QList<TransactionPtrList>::iterator i = lists.begin();
      i != lists.end(); i++)
    iterators.append(QListIterator<Transaction *>(i->rawData()));

  while(iterators.size() > 1) {
    dates.clear();
    int shouldRestart = 0;
    for(int i = 0; i < iterators.size(); i++) {
      if(! iterators[i].hasNext()) {
	iterators.removeAt(i);
	i--;
	shouldRestart = 1;
      }
      else {
	dates.append(IndexedDate(iterators[i].peekNext()->getDate(), i));
      }
    }
    if(shouldRestart)
      continue;


    // Fast enough !
    qSort(dates);

    // If the first date is too low
    if(dates[0] < dates[1]) {

      // Then, we catch up.

      QListIterator<Transaction *> & it = iterators[dates[0].i];
      while(it.hasNext() && it.peekNext()->getDate() < dates[1].d) {
	it.next();
      }
      // The earliest list is gone, starting from scratch again
      if(! it.hasNext()) {
	iterators.removeAt(dates[0].i);
	continue;
      }
      if(it.peekNext()->getDate() > dates[1].d) {
	it.next();
	continue;
      }
      
    }

    // OK, so now, we should at least have date[0] == date[1]

    // Now, we pull all the transactions with the same date into one
    // big list and we sort it.
    QDate theDate = dates[0].d;
    transactions.clear();
    for(int i = 0; i < iterators.size(); i++) {
      while(iterators[i].hasNext() && 
	    iterators[i].peekNext()->getDate() == theDate) {
	Transaction * t = iterators[i].next();
	transactions.insert(HashKey(t, permissive), t);
      }
    }

    // Now, dead easy: we loop over the keys to find some with more
    // than one value.
    tkeys = transactions.uniqueKeys();
    for(int i = 0; i < tkeys.size(); i++) {
      if(transactions.count(tkeys[i]) > 1) {
	// OK, we found possibly duplicate stuff !
	// For now, we dump them
	tvalues = transactions.values(tkeys[i]);
	if(tvalues.size() > 2) {
	  err << "findInternalMoves: " 
	      << "cannot deal with more than two matching transactions !" 
	      << endl;
	}
	else {
	  // We check the assumptions:
	  Transaction * t1, * t2;
	  t1 = tvalues[0]; t2 = tvalues[1];
	  if(! t1->account->isSameAccount(*t2->account) &&
	     (t1->getAmount() + t2->getAmount()) == 0) {
	    if(t1->links.namedLinks("internal move").count() == 0) {
	      t1->addLink(t2, "internal move");
	      retval += t1->links.namedLinks("internal move");
	    }
	  }
	}
      }
    }

  }
  
  return retval;
}
