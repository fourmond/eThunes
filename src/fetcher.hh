/** 
    \file fetcher.hh
    The attribute hash 
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


#ifndef __FETCHER_HH
#define __FETCHER_HH

/// The base class for fetching data over the internet. It is based on
/// QNetworkAccessManager and can be wrapped into a Ruby VALUE using
/// wrapForRuby();
///
/// Some ideas: QNetworkAccessManager works asynchroneously: when a
/// request is returned, it can be only progressing, until the Ruby
/// program is interested in the contents of this request, in which
/// case we wait until it is finished. This would allow for a small
/// scale parallelization of the download requests
///
/// \todo Domain restriction...
class Fetcher : public QObject {
  Q_OBJECT;

protected:

  /// The network manager
  QNetworkAccessManager * manager;

  /// The cookie Jar
  QNetworkCookieJar * cookieJar;


  /// The "Net" module
  static VALUE mNet;
  
  /// The "Fetcher" class
  static VALUE cFetcher;
  
  /// Whether the Ruby definitions of this class have been loaded.
  static bool rubyInitialized;

  /// The Ruby garbage collector.
  static void rubyFree(VALUE v);

  /// The wrapper for dummyGet
  static VALUE dummyGetWrapper(VALUE obj, VALUE str);

public:

  Fetcher();

  static void initializeRuby();

  /// Returns a Ruby object wrapping this object.
  ///
  /// \warning initializeRuby() must have been called beforehand.
  VALUE wrapToRuby();

  /// A dummy function to test things are fine for now
  void dummyGet(const QString & url);

public slots:

protected slots:
  
  void replyFinished(QNetworkReply*);
};
#endif
