/**
    \file logstorage.hh
    A storage for log messages
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


#ifndef __LOGSTORAGE_HH
#define __LOGSTORAGE_HH

#include <log.hh>


/// This class simply receives messages emitted by Log and store them,
/// for later use.
class LogStorage : public QObject {
  Q_OBJECT;

  QList<LogMessage> messages;

public:

  /// Creates the storage and connects it to the application-wide
  /// log...
  LogStorage();

  /// Returns the list of messages above the given log level
  QList<LogMessage> getMessages(Log::LogLevel l = Log::Debug);

signals:
    
  void receivedMessage(const LogMessage & message);

public slots:
  /// Adds a message
  void receiveMessage(const LogMessage & message) { 
    messages.append(message);
    emit(receivedMessage(message));
  };

  /// Clears the list of known messages.
  void clear() { messages.clear(); };

};

#endif
