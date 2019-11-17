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
  amount(am), baseTransaction(bt), previsional(false)
{
}

SerializationAccessor * AtomicTransaction::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("amount", &amount);
  ac->addScalarAttribute("comment", &comment);
  ac->addScalarAttribute("previsional", &previsional);
  addLinkAttributes(ac);
  addCategoriesSerialization(ac);
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
