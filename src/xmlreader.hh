/**
    \file link.hh
    Thin wrapper around QXmlReader for purposes of progress report
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


#ifndef __XMLREADER_HH
#define __XMLREADER_HH

/// This class is a thin wrapper around QXmlStreamReader that allows
/// running hooks every given fraction of the total stream.
class XmlReader : public QXmlStreamReader {

  qint64 totalSize;

  qint64 last;
  qint64 delta;

  
public:

  XmlReader(const QString & str);
  XmlReader(QIODevice *device);

  QXmlStreamReader::TokenType readNext();

  std::function <void (double frac) > hook;

};


#endif
