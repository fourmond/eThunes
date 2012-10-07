/**
    \file linkable.hh
    Base class for objects that can be the destination of links
    Copyright 2008, 2010, 2011 by Vincent Fourmond

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
#include <httarget.hh>

/// Base class for objects that can be the destination of links (ie
/// the ones we store); typical examples would be Transaction,
/// Document. These classes must provide a unique String ID
/// (uniqueID()), an object name, typeName(), that matches the
/// protocol used for LinksHandler, and finally a publicTypeName().
///
/// \warning For some rather obscure reasons, it seems that Linkable
/// should be the only (or first ?) base class; else the pointers
/// marshalled by LinksHandler are slightly offset... \b Hmmmm This is
/// just because I don't know how to use pointers in C++ with multiple
/// inheritance.
///
/// \todo Maybe I should write a small virtual function to provide a
/// simple textual representation of the Linkable object, such as what
/// could be used to display a tooltip, or even as the name of the
/// link ?
///
/// @todo I should go further in real, and provide pure virtual
/// functions that:
/// \li say how the link should look like (text display)
/// \li provide the functions to open a target page ? \b OR, more
/// interesting probably, simply provide any (void) callback (this
/// would allow for actions, such as "create new plugin") ? Both ?
/// \li Maybe these functionalities should join a LinkTarget base
/// class
/// 
/// This would remove most of the need for special cases in the Link
/// class.
class Linkable : public Serializable, public HTTarget {
protected:

  /// @name ID-related functions
  ///
  /// @{

  friend class SerializationAccessor;

  /// Preparation of the serializationAccessor for links
  ///
  /// @todo this should be called directly from SerializationAccessor
  void addLinkAttributes(SerializationAccessor * accessor);

  /// Adds serialization of the object ID
  void addIDSerialization(SerializationAccessor * accs);

  /// Helpers for serializing the ID
  QString objectIDGet() const;
  void objectIDSet(const QString & g);


  /// The unique ID of the object. Objects are not assigned an ID
  /// until they become the target of a link.
  int objectID;

  /// This hash contains all the (potential) targets of links, indexed
  /// by their targetID.
  static QHash<int, QSet<Linkable * > > * targets;

  /// Adds the given object to the targets hash. Does not create an
  /// ID.
  void registerSelf() const;

  /// Removes the object from the targets hash
  void unregisterSelf() const;

  /// Gets the first free object. Not MT-safe, so has to be
  /// synchronized using the appropriate mutex.
  static int getFreeID();

public:

  /// This functions makes sure that the object has a registered ID.
  int ensureHasID() const;

  /// Gets the numbered Serializable
  static Linkable * objectFromID(int id);

  /// @}

public:

  Linkable();

  Linkable(const Linkable & a);

  Linkable & operator=(const Linkable & a);

  virtual ~Linkable();

  /// The list of links.
  LinkList links;

  /// Adds a link to the given Target (and add this as a link to the
  /// target too, ie the link is bidirectionnal). It is a no-op when
  /// target is NULL.
  void addLink(Linkable * target, const QString & name = "");

  /// The number of named links:
  int hasNamedLinks(const QString & name) const;

  /// Returns the (internal) type of the object. It should match the
  /// one that LinksHandler recognizes.
  virtual QString typeName() const = 0;

  /// A 'public type name'
  virtual QString publicTypeName() const {
    return typeName();
  };

};

#endif
