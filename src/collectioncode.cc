/*
    collectioncode.cc: Implementation of CollectionCode common functions
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
#include <collectioncode.hh>
#include <transaction.hh>


/// \todo this function should use Poppler::Page::text and
/// textList... It will be much much faster, and also provide
/// rendering ?
AttributeHash CollectionCode::readPDF(QString file)
{
  AttributeHash retval;
  QStringList args;
  args << "-enc" << "UTF-8" << file << "-";
  QProcess p;
  p.start("pdftotext", args);
  p.waitForFinished();
  /// \tdexception This calls for proper error handling
  QString contents = p.readAllStandardOutput();
  retval["text"] = contents;
  args.clear();
  args << "-enc" << "UTF-8" << "-layout" << file << "-";
  p.start("pdftotext", args);
  p.waitForFinished();
  /// \tdexception This calls for proper error handling
  contents = p.readAllStandardOutput();
  retval["text-layout"] = contents;

  return retval;
}

AttributeHash CollectionCode::parseFileMetaData(const QString & doctype,
						const QString & fileName)
{
  /// \todo for now, we assume that all files are PDF.
  AttributeHash contents = readPDF(fileName);
  return parseDocumentMetaData(doctype, contents);
}

int CollectionCode::scoreForTransaction(DocumentDefinition * def,
					const AttributeHash & docMetaData,
					const AttributeHash & transaction) const
{
  // docMetaData.dumpContents();
  // transaction.dumpContents();
  if(def->relevantDate.isEmpty() || def->relevantAmount.isEmpty())
    return -10000;		// No way to match !
  int amount = docMetaData[def->relevantAmount].toInt();
  // QDate date = docMetaData[def->relevantDate].toDate();
  if(transaction["amount"].toInt() == amount)
    return 10000;
  if(! (transaction["amount"].toInt() + amount))
    return 10000;
  return -10000;
  /// \todo we should elaborate later and use the name/memo and the
  /// date distance ?
}


int CollectionCode::scoreForTransaction(Document * doc,
					Transaction * tr) const
{
  return scoreForTransaction(doc->definition, doc->attributes,
			     tr->toHash());
}
