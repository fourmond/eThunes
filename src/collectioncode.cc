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
  return retval;
}

AttributeHash CollectionCode::parseFileMetaData(const QString & doctype,
						const QString & fileName)
{
  /// \todo for now, we assume that all files are PDF.
  AttributeHash contents = readPDF(fileName);
  return parseDocumentMetaData(doctype, contents);
}
