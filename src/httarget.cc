/*
    httarget.cc: implementation of the HTTarget base class
    Copyright 2011 by Vincent Fourmond

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
#include <httarget.hh>


void HTTarget::hoverLink()
{
  /// @todo Implement tool tips !
}

QString HTTarget::toolTip()
{
  return QString();
}

QString HTTarget::encodePointer(HTTarget * pointer)
{
  char buffer[50];
  ::snprintf(buffer, sizeof(buffer), "%p", pointer);
  return QString::fromAscii(buffer);
}

HTTarget * HTTarget::decodePointer(const QString & str)
{
  HTTarget * ptr;
  ::sscanf((const char *)str.toAscii(), "%p", &ptr);
  return ptr;
}


/// @todo Implement a simple editing system with frameless popup
/// windows displayed just in place...
QString HTTarget::linkTo(const QString & str, HTTarget * pointer)
{
  return QString("<a href=\"ht:%1\">%2</a>").
    arg(encodePointer(pointer)).
    arg(str);
}
