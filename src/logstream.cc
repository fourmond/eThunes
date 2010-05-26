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

LogStream::LogStream(Log::LogLevel l, const QString & c, Log * t) : 
  level(l), channel(c)
{
  internalStream = new QTextStream(&buffer);
  if(t)
    target = t;
  else
    target = Log::logger();
}

void LogStream::flushToLog()
{
  target->logString(buffer, level, channel);
  buffer.clear();
}

LogStream & LogStream::operator<<(QTextStreamFunction t)
{
  (*internalStream) << t;
  if(t == endl || t == flush)
    flushToLog();
  return *this;
}

LogStream::~LogStream()
{
  if(! buffer.isEmpty())
    flushToLog(); 
  delete internalStream;
}
