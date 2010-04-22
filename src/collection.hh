/** 
    \file collection.hh
    Collections and their definitions
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


#ifndef __COLLECTION_HH
#define __COLLECTION_HH

#include <serializable.hh>
#include <document.hh>
#include <rubymodulecode.hh>

/// This class represents the definition of a Collection object. In
/// particular, it will hold:
/// 
/// \li a list of DocumentDefinition, to find out what kind of
/// documents are allowed
///
/// \li the code of a Ruby class... NO ! You can't define anything at
/// $SAFE = 4, and it is not reasonable to run class definition at a
/// lower level.
///
/// \todo There should be a global hash definition name ->
/// CollectionDefinition, using guarded pointers (QPointer ?)
class CollectionDefinition : public Serializable {
public:
  /// The definition of the types of documents supported by this
  /// Collection.
  QHash<QString, DocumentDefinition> documentTypes;

  /// The underlying code. For now, hardwired as RubyClassCode.
  ///
  /// \todo find a way to serialize pointers to objects and use it
  /// this way. (saving would probably not work, but that doesn't
  /// matter)
  RubyModuleCode code;

  virtual SerializationAccessor * serializationAccessor();

  /// Temporary
  void dumpContents();

  /// Loads a definition from a file.
  void loadFromFile(QString fileName);
};

/// This class holds a series of Document objects.
class Collection : public Serializable {
};

/// A group of collections, when it is convenient to group several
/// similar things ?
class CollectionFolder : public Serializable {
};
#endif
