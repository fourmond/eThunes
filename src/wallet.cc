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

void Wallet::importAccountData(const OFXImport & data)
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
    /// \todo Run filters on the new transactions !
    ac->importTransactions(data.transactions);
    ac->wallet = this;		// Make sur the wallet attribute is
				// set correctly
  }
  emit(accountsChanged());
}


SerializationAccessor * Wallet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("account", 
		   new SerializationQList<Account>(&accounts));
  ac->addAttribute("filter", 
		   new SerializationQList<Filter>(&filters));
  ac->addAttribute("category", 
		   new SerializationQHash<Category>(&categories));
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

  // Evidently
  emit(accountsChanged());
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
  emit(accountsChanged());
}

TransactionPtrList Wallet::categoryTransactions(const Category * category, 
						bool parents)
{
  TransactionPtrList vals;
  for(int i = 0; i < accounts.size(); i++)
    vals << accounts[i].categoryTransactions(category, parents);
  return vals;
}
