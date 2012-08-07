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
#include <wallet.hh>


AtomicTransaction::AtomicTransaction() :
  amount(0), category(NULL), baseTransaction(NULL)
{
  watchChild(&tags, "tags");
}

SerializationAccessor * AtomicTransaction::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("amount", &amount);
  ac->addAccessorsAttribute("category",this, 
                            &Transaction::setCategoryFromNamePrivate, 
                            &Transaction::categoryName);
  ac->addAccessorsAttribute("tags",this, 
                            &Transaction::setTagListPrivate, 
                            &Transaction::tagString);
  addLinkAttributes(ac);
  return ac;
}

Account * AtomicTransaction::getAccount() const
{
  if(baseTransaction)
    return baseTransaction->getAccount();
  return NULL;
}


QDate AtomicTransaction::getDate() const
{
  if(baseTransaction)
    return baseTransaction->getDate();
  return QDate();
}


QString AtomicTransaction::getName() const
{
  if(baseTransaction)
    return baseTransaction->getName();
  return QString();
}

QString AtomicTransaction::getMemo() const
{
  if(baseTransaction)
    return baseTransaction->getMemo();
  return QString();
}

QString AtomicTransaction::getCheckNumber() const
{
  if(baseTransaction)
    return baseTransaction->getCheckNumber();
  return QString();
}

QString AtomicTransaction::categoryName() const
{
  if(category)
    return category->fullName();
  return QString();
}

void AtomicTransaction::setCategoryFromName(const QString & str, Wallet * w)
{
  if(! w) {
    Account * account = getAccount();
    if(! account || ! account->wallet) {
      QTextStream e(stderr);
      e <<  "We have serious problems setting a Category from a Name: "
        << str << endl;
      return;
    }
    w = account->wallet;
  }
  // We create by default ?
  setCategory(w->categories.namedSubCategory(str, true));
}


void AtomicTransaction::setTagListPrivate(const QString & str) 
{
  setTagList(str, Wallet::walletCurrentlyRead);
}

void AtomicTransaction::setCategoryFromNamePrivate(const QString & str) 
{
  setCategoryFromName(str, Wallet::walletCurrentlyRead);
}

void AtomicTransaction::setTagList(const QString & str, Wallet * w)
{
  if(! w) {
    Account * account = getAccount();
    if(! account || ! account->wallet) {
      fprintf(stderr, "We have serious problems setting a "
	      "TagList from a Name\n");
      return;
    }
    w = account->wallet;
  }

  tags.fromString(str, w);
}

AttributeHash AtomicTransaction::toHash() const
{
  AttributeHash retval;
  retval["amount"] = getAmount();
  retval["date"] = getDate();
  retval["memo"] = getMemo();
  retval["name"] = getName();

  return retval;
}

int AtomicTransaction::monthID() const
{
  return monthID(getDate());
}


//////////////////////////////////////////////////////////////////////

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
  if(! checkNumber.isEmpty())
    stream << " -> check number: " << checkNumber << endl;
}

int Transaction::formatAmountModulo = 0;

Transaction::Transaction() :
  checkNumber(""),
  locked(true),
  recent(false),
  balanceMeaningful(false),
  account(NULL)
{
  watchChild(&subTransactions, "sub-transactions");
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

bool Transaction::operator==(const Transaction & t) const
{
  return (date == t.date) &&
    (name == t.name) && (amount == t.amount) && (memo == t.memo) &&
    (! account || !t.account || account->isSameAccount(*t.account)) ;
}



SerializationAccessor * Transaction::serializationAccessor()
{
  /// @todo it may be nice to implement a << -chained initialization,
  /// but that won't work really well as addAttribute needs
  /// two parameters ;-)...
  SerializationAccessor * ac = AtomicTransaction::serializationAccessor();
  ac->addScalarAttribute("date", &date);
  ac->addScalarAttribute("name", &name);
  ac->addScalarAttribute("memo", &memo);
  ac->addScalarAttribute("check-number", &checkNumber);
  ac->addListAttribute("sub", &subTransactions);
  
  return ac;
}

void Transaction::prepareSerializationRead()
{
}

void Transaction::finishedSerializationRead()
{
  for(int i = 0; i < subTransactions.size(); i++)
    subTransactions[i].baseTransaction = this;
}

int Transaction::getAmount() const
{
  int a = amount;
  for(int i = 0; i < subTransactions.size(); i++)
    a -= subTransactions[i].getAmount();
  return a;
}

bool Transaction::compareCheckNumbers(AtomicTransaction * a, 
                                      AtomicTransaction * b)
{
  return a->getCheckNumber() < b->getCheckNumber();
}


QString Transaction::transactionID() const
{
  return toHash().
    formatString("%{date%date:dd/MM/yy}##%{amount%A}##%{memo}##%{name}");
}

Account * Transaction::getAccount() const
{
  return account;
}

QList<AtomicTransaction*> Transaction::allSubTransactions()
{
  QList<AtomicTransaction *> ret;
  ret << this;
  for(int i = 0; i < subTransactions.size(); i++)
    ret << &(subTransactions[i]);
  return ret;
}
