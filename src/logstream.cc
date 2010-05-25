/*
    logstream.cc: implementation of application-wide logging facilities
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
#include <logstream.hh>

LogStream::LogStream()
{
  internalStream = new QTextStream(&buffer);
}

LogStream & LogStream::operator<<(QTextStreamFunction t)
{
  (*internalStream) << t;
  if(t == endl) {
    QTextStream o(stdout);
    o << "String logged: " << buffer;
    buffer.clear();
  }
  return *this;
}
