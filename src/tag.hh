/**
    \file tag.hh
    Class representing a Tag to which a Transaction can belong
    Copyright 2011 by Vincent Fourmond

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


#ifndef __TAG_HH
#define __TAG_HH

#include <serializable.hh>
#include <watchablecontainers.hh>

class Tag;
class Wallet;

/// A list of pointer to Tag objects.
class TagList : public WatchablePtrList<Tag> {
public:
  /// Converts the tag list into a comma-separated list.
  QString toString() const;

  /// Parses a comma-separated (whitespace around the delimiter is
  /// ignored) list of tags and store them here. A Wallet should be
  /// supplied to get the target
  void fromString(const QString & str, Wallet * wallet);

  /// Sets a Tag
  void setTag(Tag * tag);
  
  /// Clears a Tag
  void clearTag(Tag * tag);

  /// Whether the given tag is in or not.
  bool hasTag(Tag * tag) const;
};

/// A hash of Tag, intended for storage.
class TagHash : public QHash<QString, Tag> {
public:
  /// Returns a pointer to the named (sub) category, or NULL if it
  /// does not exist.
  ///
  /// On the other hand, if create is true, it is created even if it
  /// does not exist.
  ///
  /// If name is empty, returns NULL
  Tag * namedTag(const QString &name, bool create = false);

  /// Dumps the contents of the hash
  void dumpContents(QString prefix = "") const;

  /// Returns the list of tag names.
  QStringList tagNames() const {
    return keys();
  }; 
};

/// This class represents a Tag, (or a label). It is an attribute that
/// can be attached to a given Transaction (and maybe to Document
/// eventually ?). There are two main differences with Category:
/// \li they are not hierarchical: there are no "nested tag"
/// \li a given Transaction can belong to multiple tags. They are more
/// flexible than categories, but cannot be used to "partition" the
/// income, since they are not exclusive. They hence serve slightly
/// different purposes.
class Tag : public Serializable {
public:
  /// The name of this tag
  QString name;

  Tag() {;};


  virtual SerializationAccessor * serializationAccessor();

  virtual ~Tag();

};


#endif
