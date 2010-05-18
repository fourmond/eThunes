/** 
    \file fetcher.hh
    The web fecther class...
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
/// \todo Domain restriction...
///
/// \todo This class should provide a way to signal to the target
/// object (Collection, Wallet ?) that something was found.
///
/// \todo There should be one (or more ?) fetcher for each Collection
/// object (possibly created on demand using introspection facilities
/// of the code), and possibly several for a Wallet (one for
class Fetcher : public QObject {
  Q_OBJECT;

protected:

  /// Private class to handle ongoing download requests
  class OngoingRequest {
  public:
    
    /// The reply, as provided by QNetworkAccessManager
    QNetworkReply * reply;

    /// The proc object to be called upon completion of the request
    VALUE code;
  };

  /// Requests currently underway
  QHash<QNetworkReply *, OngoingRequest> ongoingRequests;


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

  /// The wrapper for get
  static VALUE getWrapper(VALUE obj, VALUE str);

  /// Spawns a get request.
  OngoingRequest * get(const QNetworkRequest & request, VALUE block);

public:

  Fetcher();

  static void initializeRuby();

  /// Returns a Ruby object wrapping this object.
  ///
  /// \warning initializeRuby() must have been called beforehand.
  VALUE wrapToRuby();

public slots:

protected slots:
  
  void replyFinished(QNetworkReply*);
};
#endif
