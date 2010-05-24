/*
    fetcher.cc: implementation of the Fetcher class
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
#include <cookiejar.hh>

////////////////////////////////////////////////////////
void CookieSet::complements(const CookieSet & other)
{
  const_iterator i = other.constBegin();
  while(i != other.constEnd()) {
    if(! contains(i.key()))
      operator[](i.key()) = i.value();
    i++;
  }
}

CookieSet & CookieSet::operator<<(const QNetworkCookie & cookie)
{
  operator[](cookie.name()) = cookie;
  return *this;
}

QList<QNetworkCookie> CookieSet::toList() const
{
  QList<QNetworkCookie> ret;
  const_iterator i = constBegin();
  while(i != constEnd()) {
    ret << i.value();
    i++;
  }
  return ret;
}

void CookieSet::dumpTree(QTextStream & stream,
			 const QString & prefix) const
{
  QStringList names = keys();
  names.sort();
  for(int i = 0; i < names.size(); i++)
    stream << prefix << names[i] << " = "
	   << value(names[i]).value() << endl;
}


////////////////////////////////////////////////////////////

QString CookiesByPath::normalizePath(const QString & path)
{
  if(! path.startsWith("/")) {
    return QString("/") + path;
  }
  return path;
}


QStringList CookiesByPath::subPaths(const QString & path)
{
  QString rp = path;
  while(rp.startsWith("/"))
    rp.remove(0,1);
  QStringList paths = rp.split("/");
  QStringList ret;
  while(paths.size()) {
    ret << normalizePath(paths.join("/"));
    paths.removeLast();
  }
  ret << "/"; 			// Always
  return ret;
}

CookieSet CookiesByPath::cookiesForPath(const QString & path) const
{
  QStringList paths = subPaths(path);
  CookieSet ret;
  // QTextStream o(stdout);
  // o << "Looking into these paths for " << path << endl;
  // o << paths.join(", ") << endl;

  for(int i = 0; i < paths.size(); i++)
    if(contains(paths[i]))
      ret.complements(value(paths[i]));
  return ret;
}

CookiesByPath & CookiesByPath::operator<<(const QNetworkCookie & cookie)
{
  QString path = normalizePath(cookie.path());
  operator[](path) << cookie;
  return *this;
}

QList<QNetworkCookie> CookiesByPath::allCookies() const
{
  const_iterator i = constBegin();
  QList<QNetworkCookie> list;
  while(i != constEnd()) {
    list += i.value().toList();
    i++;
  }
  return list;
}

void CookiesByPath::dumpTree(QTextStream & stream,
			     const QString & prefix) const
{
  QStringList paths = keys();
  paths.sort();
  for(int i = 0; i < paths.size(); i++) {
    stream << prefix << "Path: " << paths[i] << endl;
    value(paths[i]).dumpTree(stream, prefix + "\t");
  }
}


////////////////////////////////////////////////////////

QStringList CookiesByHost::subHosts(const QString & host)
{
  QStringList hosts;
  QStringList components = host.split(".");
  hosts << host;
  components.removeFirst();
  while(components.size() > 1) {
    hosts << QString(".") + components.join(".");
    components.removeFirst();
  }
  return hosts;
}

CookiesByHost & CookiesByHost::operator<<(const QNetworkCookie & cookie)
{
  operator[](cookie.domain()) << cookie;
  return *this;
}

CookieSet CookiesByHost::cookiesForUrl(const QUrl & url) const
{
  QStringList hosts = subHosts(url.host());
  CookieSet ret;
  for(int i = 0; i < hosts.size(); i++)
    if(contains(hosts[i]))
      ret.complements(value(hosts[i]).cookiesForPath(url.path()));
  return ret;
}


QList<QNetworkCookie> CookiesByHost::allCookies() const
{
  const_iterator i = constBegin();
  QList<QNetworkCookie> list;
  while(i != constEnd()) {
    list += i.value().allCookies();
    i++;
  }
  return list;
}

void CookiesByHost::dumpTree(QTextStream & stream,
			     const QString & prefix) const
{
  QStringList domains = keys();
  domains.sort();
  for(int i = 0; i < domains.size(); i++) {
    stream << prefix << "Domain: " << domains[i] << endl;
    value(domains[i]).dumpTree(stream, prefix + "\t");
  }
}

////////////////////////////////////////////////////////
QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl & url) const
{
  QList<QNetworkCookie> l = cookies.cookiesForUrl(url).toList();
  QList<QNetworkCookie> l2;
  for(int i = 0; i < l.size(); i++) {
    const QNetworkCookie &c = l[i];
    if(c.isSecure() && url.scheme() != "https")
      continue;			// Drop secure cookies on non-https
				// requests
    l2 << c;
  }
  return l2;
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookieList,
				  const QUrl & url)
{
  for(int i = 0; i < cookieList.size(); i++) {
    QNetworkCookie c = cookieList[i];
    if(c.domain().isEmpty())	// Ensure belongs only to host if no
				// domain specified.
      c.setDomain(url.host());
    // Nothing done on the path.
    cookies << c;
  }
  return true;
}

void CookieJar::dumpCookieList(const QList<QNetworkCookie> &list)
{
  QTextStream out(stdout);
  for(int i = 0; i < list.size(); i++) {
    const QNetworkCookie & c = list[i];
    QString value = c.value();
    if(value.size() > 30)
      value = value.left(30) + "...";
    out << "\t" << c.name() << " : " << value
	<< " (" << c.domain() << " - " << c.path() << ")";
    if(c.isSecure())
      out << " SECURE";
    out << endl;
  }
}
