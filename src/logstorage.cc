/*
    logstorage.cc: implementation of application-wide logging facilities
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
#include <logstorage.hh>

LogStorage::LogStorage() 
{
  /// @todo have the possibility to choose either html or plain ?
  connect(Log::logger(), SIGNAL(htmlMessage(const LogMessage &)),
	  SLOT(receiveMessage(const LogMessage &)));
}


QList<LogMessage> LogStorage::getMessages(Log::LogLevel l)
{
  QList<LogMessage> msg;
  for(int i = 0; i < messages.size(); i++)
    if(messages[i].level >= l)
      msg.append(messages[i]);
  return msg;
}
