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

class LogStream;

/// The log system of QMoney is composed of two different parts
/// 
/// \li an application wide instance of Log, whose role is to get
/// string data from various sources, classifying them though channels
/// (possibly to be implemented later on) and severities, with the
/// standard meaning. This class implements a signalling scheme making
/// it easy to simply listen to some log levels/severities.
///
/// \li LogStream streams that send their data to a given channel and severity
/// level (that optionally could be changed ?)
class Log : public QObject {

  /// The application-wide logger
  static Log * theUniqueLogger;
public:

  static Log * logger();
  

  static LogStream biniou();

signals:
};

#endif
