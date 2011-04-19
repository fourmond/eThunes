/**
    \file fetcher.hh
    The web fetcher class...
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

#include <attributehash.hh>
#include <cookiejar.hh>

// Targets
class Collection;
class Wallet;

// Result
class Result;

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
/// of the code), and possibly several for a Wallet (one for each
/// backend ?)
///
/// \todo The best way to organize everything would be to organize
/// things according to 'logins': the ruby code would be asked to
/// kindly fetch information for a given 'login', possibly several
/// times in a row with a different login, and with a different
/// fetcher object. The Ruby code should be passed a hash of 'document
/// type' => array of document hashes to ensure that as far as is
/// reasonably possible, documents are not downloaded/parsed twice.
///
/// \todo There should be an addDocument function that would do:
/// \li add the document if the target is a Collection
/// \li or import the OFX file (or whatever is actually is) if the
/// target is an account...
class Fetcher : public QObject {
  Q_OBJECT;

protected:

  static inline Fetcher * fromValue(VALUE v) {
    Fetcher * f;
    Data_Get_Struct(v,Fetcher,f);
    return f;
  };


  /// Private class to handle ongoing download requests
  class OngoingRequest {
  public:

    /// The reply, as provided by QNetworkAccessManager
    QPointer<QNetworkReply> reply;

    /// The proc object to be called upon completion of the request
    VALUE code;

    /// Whether the request processing has ended or not.
    ///
    /// \todo Eventually, it would be good to implement a signalling
    /// mechanism to tell when the Fetcher has finished all its
    /// pending jobs (meaning as well that it can be disposed of).
    bool done;

    /// maximum number of redirections left
    int maxHops;
  };

  /// Register a request in the hash, and setup the right things about
  /// it.
  OngoingRequest * registerRequest(QNetworkReply * reply,
				   VALUE code, int redirections = 0);

  /// Requests currently underway
  QHash<QNetworkReply *, OngoingRequest> ongoingRequests;


  /// The network manager
  QNetworkAccessManager * manager;

  /// The cookie Jar
  CookieJar * cookieJar;


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
  OngoingRequest * get(const QNetworkRequest & request, VALUE block,
		       int redirections = 0);

  /// The wrapper for post
  static VALUE postWrapper(VALUE obj, VALUE str, VALUE hash);


  /// The wrapper for post
  static VALUE cookiesWrapper(VALUE obj);

  /// Spawns a post request with the given parameters.
  OngoingRequest * post(const QNetworkRequest & request,
			const AttributeHash & params,
			VALUE block);


  // /// The wrapper for post
  // static VALUE cookiesWrapper(VALUE obj);

  // /// A very basic wrapping for cookies
  // AttributeHash cookies();

  /// Whether or not the Fetcher should follow redirections (by
  /// sending GET requests). On by default, as this is what we
  /// generally want.
  bool followRedirections;


  /// Wallet target. Only one of targetWallet and targetCollection
  /// shouldn't be NULL
  Wallet * targetWallet;

  /// Target Collection
  Collection * targetCollection;

  /// Adds a document to the target of the Fetcher object (ie, either
  /// the Wallet or the Collection). The contents of the Result are
  /// fed to the target.
  ///
  /// Return value is true if document was added, false otherwise.
  bool addDocument(Result * result, const QString & doctype);

  /// The wrapper for addDocument()
  static VALUE addDocumentWrapper(VALUE obj, VALUE result,
				  VALUE doctype);

public:

  void setTarget(Collection * c) {
    targetCollection = c;
    targetWallet = 0;
  };

  void setTarget(Wallet * w) {
    targetCollection = 0;
    targetWallet = w;
  };

  /// \todo Eventually (soon) a Fetcher class should target a
  /// collection ? Or something else ? (a common base class of
  /// Collection and Wallet ? ) Or simply a choice between Wallet and
  /// Collection that would be handled as in AccountModel
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
