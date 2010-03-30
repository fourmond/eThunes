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
    ac->importTransactions(data.transactions);
  }
}


SerializationAccessor * Wallet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("account", 
		   new SerializationQList<Account>(&accounts));
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
