/*
    ofximport.cc: A simple way to import OFX 'downloads' from banks.
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
#include <ofximport.hh>

#include <logstream.hh>

OFXImport OFXImport::importFromFile(const QString &file)
{
  QFile f(file);
  LogStream log(Log::Info);
  f.open(QIODevice::ReadOnly);
  log << "Reading transactions from file " << file << endl;
  return importFromFile(&f);
}


OFXImport OFXImport::importFromFile(QIODevice * stream)
{
  // LogStream debug(LogLevel::Debug);
  LogStream info(Log::Info);

  // A regular expression that matches a header string.
  QRegExp headerRE("^\\s*(\\w+):(.*)$");
  // A regular expression that matches a tag
  QRegExp tagRE("^\\s*<(\\w+)>(.*)?$");

  // A regular expression that matches the end of a tag
  QRegExp endOfTagRE("^\\s*</(\\w+)>");

  // A regular expression matching a date ??
  QRegExp dateRE("(\\d{4})(\\d{2})(\\d{2})");

  QByteArray line;

  int readingHeader = 1;

  Transaction * currentTransaction = NULL;

  Account * currentAccount = NULL;

  OFXImport retVal;


  /// Reads up to but not including the next <
  auto getNext = [&stream]() -> QByteArray {
    QByteArray s;
    char c;
    while(stream->getChar(&c)) {
      if(c == '<' && s.size() > 0) {
        stream->ungetChar(c);
        break;
      }
      s.append(c);
    }
    return s;
  };
  

  while(true) {
    line = getNext();
    if(line.isEmpty())
      break;
    if(tagRE.indexIn(line) == 0) {
      // debug << "Tag: " << tagRE.cap(1) << endl;
      // Now, we look what is the tag

      // First, transaction-related tags:
      if(tagRE.cap(1).compare("STMTTRN", Qt::CaseInsensitive) == 0) {
	// Beginning of a transaction
	retVal.transactions.append(Transaction());
	currentTransaction = & (retVal.transactions.last());
	currentTransaction->account = currentAccount;
      }
      else if(tagRE.cap(1).compare("DTPOSTED", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction) {
	  if(dateRE.indexIn(tagRE.cap(2)) >= 0)
	    currentTransaction->date = QDate(dateRE.cap(1).toInt(),
					     dateRE.cap(2).toInt(),
					     dateRE.cap(3).toInt());
	}
      }
      else if(tagRE.cap(1).compare("NAME", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction)
	  currentTransaction->name = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("MEMO", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction)
	  currentTransaction->memo = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("CHECKNUM", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction)
	  currentTransaction->checkNumber = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("TRNAMT", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction) {
	  QStringList a = tagRE.cap(2).split(QRegExp("[.,]"));
	  bool negative = a[0].contains("-");
	  int amount = a[0].toInt() * 100;
	  if(negative)
	    amount -= a[1].toInt();
	  else
	    amount += a[1].toInt();
	  currentTransaction->amount = amount;
	}
      }

      // Now, account-related tags
      else if(tagRE.cap(1).compare("BANKACCTFROM", Qt::CaseInsensitive) == 0) {
	retVal.accounts.append(Account());
	currentAccount = & (retVal.accounts.last());
      }
      else if(tagRE.cap(1).compare("BANKID", Qt::CaseInsensitive) == 0) {
	if(currentAccount)
	  currentAccount->bankID = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("BRANCHID", Qt::CaseInsensitive) == 0) {
	if(currentAccount)
	  currentAccount->branchID = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("ACCTID", Qt::CaseInsensitive) == 0) {
	if(currentAccount)
	  currentAccount->accountNumber = tagRE.cap(2).trimmed();
      }
      else if(tagRE.cap(1).compare("ACCTTYPE", Qt::CaseInsensitive) == 0) {
	if(currentAccount) {
	  if(tagRE.cap(2).trimmed().compare("SAVINGS",
					    Qt::CaseInsensitive) == 0)
	    currentAccount->type = Account::Savings;
	}
      }


    }
    else if(endOfTagRE.indexIn(line) == 0) {
      // Now, we look what is the tag we've just finished to parse...
      if(endOfTagRE.cap(1).compare("STMTTRN", Qt::CaseInsensitive) == 0) {
	if(currentTransaction)
	  currentTransaction = NULL;
      }
    }
  }
  info << "Imported " << retVal.transactions.size() 
       << " transactions spanning " << retVal.accounts.size() 
       << " accounts" << endl;
  return retVal;
}

void OFXImport::testImport(const QString & file)
{
  importFromFile(file); 
}
