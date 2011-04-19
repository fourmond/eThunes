/*
    log.cc: implementation of application-wide logging facilities
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
#include <log.hh>

Log::Log() : spy(0)
{
}

Log * Log::theUniqueLogger = 0;

Log * Log::logger()
{
  if(! theUniqueLogger)
    theUniqueLogger = new Log();
  return theUniqueLogger;
}

QString Log::logLevelName(LogLevel l)
{
  switch(l) {
  case Debug: return tr("debug");
  case Info: return tr("info");
  case Warning: return tr("warning");
  case Error: return tr("error");
  };
  return "????";
}

void Log::logString(const QString & message, LogLevel l,
		    const QString & channel)
{
  QString final = QString("[") + logLevelName(l);
  if(! channel.isEmpty())
    final += ", " + channel;
  final += "] " + message;
  QString plain = toPlainText(final);
  emit(plainMessage(LogMessage(plain, l, channel)));
  emit(htmlMessage(LogMessage(toHTML(final), l, channel)));
  if(spy) {
    spy->write(plain.toLocal8Bit());
    spy->flush();
  }
}

QString Log::toHTML(const QString &str)
{
  return str;
}

QString Log::toPlainText(const QString & str)
{
  return str;
}
