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


AtomicTransaction::AtomicTransaction(int am, Transaction * bt) :
  amount(am), category(NULL), baseTransaction(bt)
{
  watchChild(&tags, "tags");
}

SerializationAccessor * AtomicTransaction::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("amount", &amount);
  ac->addScalarAttribute("comment", &comment);
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

void AtomicTransaction::setTagFromName(const QString & str, Wallet * w)
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
  Tag * t = w->tags.namedTag(str, true);
  setTag(t);
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

QString AtomicTransaction::publicLinkName() const
{
  QString rv = QObject::tr("T: %1 %2").
    arg(Transaction::formatAmount(getAmount())).
    arg(getDate().toString("dd/MM/yy"));
  return rv;
}
