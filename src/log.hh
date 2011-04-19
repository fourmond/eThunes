/**
    \file log.hh
    Log class for application-wide logging
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


#ifndef __LOG_HH
#define __LOG_HH


class LogMessage;

/// The log system of eThunes is composed of two different parts
///
/// \li an application wide instance of Log, whose role is to get
/// string data from various sources, classifying them though channels
/// (possibly to be implemented later on) and severities, with the
/// standard meaning. This class implements a signalling scheme making
/// it easy to simply listen to some log levels/severities.
///
/// \li LogStream streams that send their data to a given channel and severity
/// level (that optionally could be changed ?)
///
/// \todo Now, what should be done is to create a simple logbrowser
/// dialog box that would be created first thing from the mainwindow's
/// constructor and that would display the log acquired so far.
class Log : public QObject {
  Q_OBJECT;

  /// The application-wide logger
  static Log * theUniqueLogger;

  friend class LogStream;

  /// Converts an HTML-formatted text that potentially contains <raw>
  /// tags into a text suitable for plain text output (specifically to
  /// stdout).
  static QString toPlainText(const QString &txt);

  /// Does the same thing, but for HTML output.
  static QString toHTML(const QString &txt);

public:

  Log();

  static Log * logger();

  typedef enum {
    Debug,
    Info,
    Warning,
    Error
  } LogLevel;

  static QString logLevelName(LogLevel l);

  /// IF this pointer isn't NULL, all plain log messages are written
  /// out there.
  ///
  /// \todo This is more a hack than anything else, but it
  /// will surely help in a close future.
  ///
  /// We use a file to ensure the existence of a flush !
  QFile * spy;


signals:
  /// \todo There should be two kind of signals:
  /// \li one that just forwards all incoming messages of logString()
  ///
  /// \li one that filters according to a policy that should be
  /// implemented somewhere ?

  /// A plain text formatted message, such as would be suitable for
  /// display in stdin
  void plainMessage(const LogMessage & message);

  /// The message as would be suitable for display in an HTML log
  /// browser.
  void htmlMessage(const LogMessage & message);

protected:
  /// Logs a string; in particular, in real this just sends a signal
  /// everywhere.
  ///
  /// \todo Maybe this should also write to a file ? Although there
  /// would be no reason why, we just need a proper object with the
  /// proper slots...
  ///
  /// This function accepts HTML input in message, with a small
  /// additional feature: a string within <raw>...</raw> is left
  /// untouched to text output, and tweaked to be HTML-safe for HTML
  /// output.
  ///
  /// \warning This is internal specification and could change at any
  /// time. Use stream manipulators !
  void logString(const QString & message, LogLevel l,
		 const QString & channel = QString());

};

/// A log message
class LogMessage {
public:
  /// The text of the message
  QString message;

  /// The log level
  Log::LogLevel level;

  /// The channel (unused for now ?)
  QString channel;

  LogMessage() : level(Log::Debug) { ;};

  LogMessage(const QString & msg, Log::LogLevel l, 
	     const QString & c) : message(msg), level(l), channel(c) { ;};
};

Q_DECLARE_METATYPE(LogMessage);

#endif
