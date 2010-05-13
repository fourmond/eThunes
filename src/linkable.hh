/** 
    \file linkable.hh
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


#ifndef __LINKABLE_HH
#define __LINKABLE_HH

#include <link.hh>

/// Base class for objects that can be the destination of links (ie
/// the ones we store); typical examples would be Transaction,
/// Document. These classes must provide a unique String ID
/// (uniqueID()), an object name
class Linkable : public Serializable {
protected:
  /// Preparation of the serializationAccessor for links
  void addLinkAttributes(SerializationAccessor * accessor);

public:

  /// The list of links.
  LinkList links;

  /// Adds a link to the given Target (and add this as a link to the
  /// target too). It is a no-op when target is NULL.
  void addLink(Linkable * target);

  /// Returns the unique String ID for this object, suitable for
  /// serialization.
  virtual QString uniqueID() const = 0;

  /// Returns the (internal) type of the object. It should match the
  /// one that LinksHandler recognizes.
  virtual QString typeName() const = 0;

  /// A 'public type name'
  virtual QString publicTypeName() const {
    return typeName();
  };

};

#endif
