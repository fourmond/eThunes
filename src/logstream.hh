/**
    \file logstream.hh
    LogStream class: data stream for logging;
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


#ifndef __LOGSTREAM_HH
#define __LOGSTREAM_HH

#include <log.hh>

/// Use this class just as you would use a QTextStream.
///
/// \todo I should write up some manipulators and some functions to
/// tweak the format of the output (in particular, bold, color, and
/// the like...)
class LogStream {

  /// The internal stream used to build up the string to be forwarded
  /// to Log.
  QTextStream * internalStream;

  /// The target string
  QString buffer;

  /// The target Log object
  Log * target;

  /// The log level
  Log::LogLevel level;

  /// The log channel
  QString channel;

  /// Sends out to the target
  void flushToLog();
public:

  /// Builds up a log stream with the given level, channel and target.
  ///
  /// Mostly, you will only ever need something like that:
  ///   LogStream o(); // or
  ///   LogStream o(Log::debug);
  LogStream(Log::LogLevel level = Log::Info,
	    const QString & channel = QString(),
	    Log * target = 0);

  ~LogStream();

  void setLogLevel(Log::LogLevel l) {
    level = l;
  };

  Log::LogLevel logLevel() const { return level;};

  template<typename T> LogStream & operator<<(const T& t) {
    (*internalStream) << t;
    return *this;
  };

  LogStream & operator<<(QTextStreamFunction t);

};

#endif
