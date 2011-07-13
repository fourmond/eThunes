/*
    wallet.cc: The Wallet class
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
#include <wallet.hh>


Wallet::Wallet()
{
  watchChild(&accounts, "accounts");
  watchChild(&filters, "filters");
  watchChild(&accountGroups, "groups");
}

void Wallet::importAccountData(const OFXImport & data, bool runFilters)
{
  for(int i = 0; i < data.accounts.size(); i++) {
    Account * ac = 0;
    int j = 0;
    for(; j < accounts.size(); j++)
      if(accounts[j].isSameAccount(data.accounts[i]))
	ac = &accounts[j];
    if(! ac) {
      accounts.append(data.accounts[i]);
      ac = &accounts[j];
    }
    ac->importTransactions(data.transactions,
			   runFilters ? this : NULL);
    ac->wallet = this;		// Make sure the wallet attribute is
				// set correctly
  }
}


SerializationAccessor * Wallet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addListAttribute("account", &accounts);
  ac->addListAttribute("group", &accountGroups);
  ac->addListAttribute("filter", &filters);
  ac->addHashAttribute("category", &categories);
  ac->addHashAttribute("tag", &tags);
  return ac;
}

void Wallet::saveToFile(QString fileName)
{
  QFile file(fileName);
  file.open(QIODevice::WriteOnly);
  QXmlStreamWriter w(&file);
  w.setAutoFormatting(true);
  w.setAutoFormattingIndent(2);
  w.writeStartDocument();
  writeXML("wallet", &w);
  w.writeEndDocument();
}


void Wallet::loadFromFile(QString fileName)
{
  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QXmlStreamReader w(&file);
  while(! w.isStartElement() && ! w.atEnd())
    w.readNext();
  readXML(&w);
}

void Wallet::clearContents()
{
  accounts.clear();
  filters.clear();
  categories.clear();
}


Wallet * Wallet::walletCurrentlyRead = NULL;

void Wallet::prepareSerializationRead()
{
  clearContents();
  walletCurrentlyRead = this;
}

void Wallet::finishedSerializationRead()
{
  walletCurrentlyRead = NULL;
  // Make sure that account.wallet points to here.
  for(int i = 0; i < accounts.size(); i++)
    accounts[i].wallet = this;

  // Then, make sure all the AccountGroup have correct pointers
  for(int i = 0; i < accountGroups.size(); i++)
    accountGroups[i].finalizePointers(this);
}

void Wallet::runFilters(TransactionList * list)
{
  for(int i = 0; i < filters.size(); i++)
    filters[i].processList(list);
}

void Wallet::runFilters()
{
  for(int i = 0; i < accounts.size(); i++)
    runFilters(&accounts[i].transactions);
}

TransactionPtrList Wallet::transactionsForFilter(const Filter * filter)
{
  TransactionPtrList ret;
  for(int i = 0; i < accounts.size(); i++)
    ret.append(filter->matchingTransactions(&accounts[i].transactions));
  return ret;
}

TransactionPtrList Wallet::categoryTransactions(const Category * category,
						bool parents)
{
  TransactionPtrList vals;
  for(int i = 0; i < accounts.size(); i++)
    vals.append(accounts[i].categoryTransactions(category, parents));
  return vals;
}

TransactionPtrList Wallet::transactionsWithinRange(const QDate & before, const QDate & after)

{
  TransactionPtrList list;
  for(int i = 0; i < accounts.size(); i++)
    list.append(accounts[i].transactions.transactionsWithinRange(before,after));
  return list;
}

Account * Wallet::namedAccount(const QString & name)
{
  for(int i = 0; i < accounts.size(); i++)
    if(accounts[i].accountID() == name)
      return &accounts[i];
  return NULL;
}

Transaction * Wallet::namedTransaction(const QString & name)
{
  int idx = name.indexOf("##");
  QString accountID = name.left(idx);
  QString transactionID = name.mid(idx + 2);
  Account * account = namedAccount(accountID);
  if(account)
    return account->namedTransaction(transactionID);
  return NULL;
}

Category * Wallet::namedCategory(const QString & name)
{
  return categories.namedSubCategory(name, false);
}

Tag * Wallet::namedTag(const QString & name)
{
  TagHash::iterator i = tags.find(name);
  if(i != tags.end())
    return &(i.value());
  return NULL;
}

void Wallet::findInternalMoves()
{
  QList<TransactionPtrList> lists;
  for(int i = 0; i < accounts.size(); i++) {
    lists.append(accounts[i].transactions.toPtrList());
  }
  TransactionPtrList::findInternalMoves(lists);
}

