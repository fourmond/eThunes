/**
    \file link.hh
    Base class for objects that can be the destination of links
    Copyright 2008,2010 by Vincent Fourmond

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


#ifndef __LINK_HH
#define __LINK_HH

#include <serializable.hh>

class Linkable;
class Cabinet;

class Link : public Serializable {
public:
  Link() : target(NULL) {;};
  Link(Linkable * t, const QString & name) : 
    target(t), linkName(name) {;};

  /// The target of the link
  Linkable * target;

  /// A name attached to the target
  QString linkName;

  virtual SerializationAccessor * serializationAccessor();

  virtual void finishedSerializationRead();
  virtual void prepareSerializationWrite();

  /// This functions browses the contents of linksToBeFinalized and
  /// finalizes the target attribute based on linkID and typeName. It
  /// must be called in Cabinet::finishedSerializationRead(). It
  /// returns the number of links that were found dangling (ie NULL);
  /// if there is any, we should process all the Linkable objects and
  /// repair by ensuring that a link is always reciprocal.
  static int finalizePendingLinks(Cabinet * cabinet);

protected:
  /// The ID of the link as stored on disk; the value of
  /// Linkable::uniqueID().
  QString linkID;

  /// The type name, as in Linkable::typeName()
  QString typeName;

  /// This is a global list of links whose target should be adjusted
  /// after everything has been serialized.
  static QList<Link*> linksToBeFinalized;
};


/// A list of links.
class LinkList : public QList<Link> {
public:
  /// Adds a link to the given target, making sure there are no
  /// duplicates.
  void addLink(Linkable * target, const QString & name = "");

  /// Returns a list of html links suitable for use with LinksHandler.
  /// You just need to join the result to get a decent string.
  QStringList htmlLinkList() const;

  /// Returns a list of links bearing the given name
  QList<Link *> namedLinks(const QString & name);
  QList<const Link *> namedLinks(const QString & name) const;

};

#endif
