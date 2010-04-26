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

/// This class represents the definition of a Collection object. 
///
///
/// \todo There should be a global hash definition name ->
/// CollectionDefinition, using guarded pointers (QPointer ?)
///
/// \todo There should be a way to load all definitions available to
/// the program (or at least to browse them easily ?): this means
/// setting up a path, providing function to load a file based on its
/// name (which would also be inside the file ? No ? Yes ?) and to
/// enumerate all the files found.
///
/// \todo Add attributes to identify the author, and possibly
/// copyright, licenses, and so on...
class CollectionDefinition : public Serializable {
public:
  /// The definition of the types of documents supported by this
  /// Collection.
  QHash<QString, DocumentDefinition> documentTypes;

  /// The underlying code. For now, hardwired as RubyClassCode.
  ///
  /// \todo find a way to serialize pointers to objects and use it
  /// this way. It would be enough to write a small class that
  /// serializes a pointer to a Serializable object; several
  /// statements could be used for the different types, with different
  /// element names. This would work for loading, but not for saving !
  ///
  /// Maybe use a hash 'element name' => Serializable ? (with full
  /// conversion ? but then finding out which one to use for writing
  /// out isn't that trivial). Or let that be handled by virtual
  /// functions ? but then it makes duplicated code (for the choice of
  /// the element at saving and the choice of the target class at
  /// loading)
  RubyModuleCode code;

  /// A public name with no particular restriction
  QString name;

  /// A public description
  QString description;

  /// A restricted domain: HTTP access from the code of the definition
  /// should not be done outside of this domain (strings prepended
  /// with a . before the domain are fine, though).
  ///
  /// Of course, so long as the Ruby code can setup sockets on its
  /// own, no guarantee can be made with respect to what can be
  /// accessed. But, still !
  QString domain;

  virtual SerializationAccessor * serializationAccessor();

  /// Temporary
  void dumpContents();

  /// Loads a definition from a file.
  void loadFromFile(QString fileName);
};

/// This class holds a series of Document objects.
class Collection : public Serializable {
public:

  /// The underlying definition for this collection.
  CollectionDefinition * definition;
  
  /// Returns the file name format for the given Document.
  ///
  /// \todo Here, implement user overrides.
  QString documentFileNameFormat(Document * doc) {
    return doc->definition->fileNameFormat;
  };

  /// Returns the display format for the given Document.
  ///
  /// \todo Here, implement user overrides.
  QString documentDisplayFormat(Document * doc) {
    return doc->definition->displayFormat;
  };

  /// All the documents, regardless of their type.
  QList<Document> documents;

  /// \todo Returns the list of documents with the named type.
  QList<Document *> typeDocuments(const QString & name);

  /// Imports a file as a document of the given type, and returns a
  /// pointer to the newly-created document (whose ownership stays
  /// within the Collection).
  Document * importFile(const QString & doctype, const QString & file);


  virtual SerializationAccessor * serializationAccessor();
};

/// A group of collections, when it is convenient to group several
/// similar things ? Is it necessary ?
class CollectionFolder : public Serializable {
};
#endif
