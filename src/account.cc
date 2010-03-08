/*
    account.cc: Collection of objects that represent an account.
    Copyright 2008 by Vincent Fourmond

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
