/** 
    \file cookiejar.hh
    The web cookiejar class...
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


#ifndef __COOKIEJAR_HH
#define __COOKIEJAR_HH

/// A set of cookies indexed by their names.
class CookieSet : public QHash<QString, QNetworkCookie> {
public:
  /// Complements this set of cookies by the contents of the other
  /// set, not overriding cookies that are already present.
  void complements(const CookieSet & other);

  /// Adds a cookie into this set; no check on the name is performed:
  /// new cookies override old ones.
  CookieSet & operator<<(const QNetworkCookie & cookie);

  /// Converts this hash into a flat list.
  QList<QNetworkCookie> toList() const;
};

/// A set of CookieSet indexed by their path.
class CookiesByPath : public QHash<QString, CookieSet> {
 
  /// Returns a list of Paths relevant for the given path, in the
  /// reverse order of their relevance.
  static QStringList subPaths(const QString & path);

  /// Makes sure the path starts with a /;
  static QString normalizePath(const QString & path);

public:
  /// Adds a cookie into this set
  CookiesByPath & operator<<(const QNetworkCookie & cookie);

  /// Returns all cookies for the given path: the most relevant are
  /// added first, and are completed (see CookieSet::complements) by
  /// less relevant ones.
  CookieSet cookiesForPath(const QString & path) const;

};

/// A set of CookiesByPath indexed by their host
class CookiesByHost : public QHash<QString, CookiesByPath> {
  /// Returns a list of "host" name matching the given host, from the
  /// most relevant to the least relevant.
  static QStringList subHosts(const QString & host);

public:

  /// Adds a cookie into this set
  CookiesByHost & operator<<(const QNetworkCookie & cookie);

  /// Returns all cookies relevant for the given URL (ie the proper
  /// host and the proper paths and so on...)
  CookieSet cookiesForUrl(const QUrl & url) const;

};

/// This is a CookieJar 
class CookieJar : public QNetworkCookieJar {

  CookiesByHost cookies;

public:

  virtual QList<QNetworkCookie>	cookiesForUrl(const QUrl & url) const;
  virtual bool setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, 
				 const QUrl & url);
};
#endif
