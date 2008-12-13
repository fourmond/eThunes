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

OFXImport OFXImport::importFromFile(QString file)
{
  QFile f(file);
  f.open(QIODevice::ReadOnly);
  return importFromFile(&f);
}


OFXImport OFXImport::importFromFile(QIODevice * stream)
{
  // Allright, fun starts here ;-) !


  // Debugging output:
  QTextStream debug(stderr);
  
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

  while(1) {
    line = stream->readLine();
    if(line.isEmpty())
      break;
    if(readingHeader) {
      if(headerRE.indexIn(line) == 0) {
	debug << "Header tag: " << headerRE.cap(1)
	      << " -> " << headerRE.cap(2) << endl;
	continue;
      }
      else
	readingHeader = 0;
    }
    if(tagRE.indexIn(line) == 0) {
      // debug << "Tag: " << tagRE.cap(1) << endl;
      // Now, we look what is the tag
      if(tagRE.cap(1).compare("STMTTRN", Qt::CaseInsensitive) == 0)
	// Beginning of a transaction
	currentTransaction = new Transaction;
      else if(tagRE.cap(1).compare("DTPOSTER", Qt::CaseInsensitive) == 0) {
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
	  currentTransaction->checkNumber = tagRE.cap(2).toInt();
      }
      else if(tagRE.cap(1).compare("TRNAMT", Qt::CaseInsensitive) == 0) {
	// The date of the transaction
	if(currentTransaction)
	  // Quite naive, but it really should work most of the times...
	  currentTransaction->amount = tagRE.cap(2).toDouble() * 100;
      }
    }
    else if(endOfTagRE.indexIn(line) == 0) {
      // Now, we look what is the tag we've just finished to parse...
      if(endOfTagRE.cap(1).compare("STMTTRN", Qt::CaseInsensitive) == 0) {
	if(currentTransaction) {
	  currentTransaction->dump(debug);
	  delete currentTransaction;
	  currentTransaction = NULL;
	}
      }
    }
  }
  return OFXImport();
}
