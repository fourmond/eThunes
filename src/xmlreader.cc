/*
    xmlreader.cc: implementation of XmlReader
    Copyright 2017 by Vincent Fourmond

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
#include <xmlreader.hh>

XmlReader::XmlReader(const QString & str) :
  QXmlStreamReader(str), last(0)
{
  totalSize = str.size();
  delta = totalSize / 100 + 1;
}

XmlReader::XmlReader(QIODevice * device) :
  QXmlStreamReader(device), last(0)
{
  totalSize = device->size();
  delta = totalSize / 100 + 1;
}

QXmlStreamReader::TokenType XmlReader::readNext()
{
  QXmlStreamReader::TokenType t = QXmlStreamReader::readNext();
  qint64 cur = characterOffset();
  if(hook && last/delta < cur/delta)
    hook(cur*1.0/totalSize);
  last = cur;
  return t;
}


