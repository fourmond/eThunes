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

#include <collectiondefinition.hh>

#include <watchablecontainers.hh>
#include <httarget.hh>

class Cabinet;

/// This class holds a series of Document objects.
///
/// \todo Write a small class to serialize a CollectionDefinition *,
/// saving as definition->name and loading using namedDefinition,
/// (eventually) raising an exception when NULL is returned.
class Collection : 
  public Serializable, public HTTarget {
public:

  /// The Cabinet this collection belongs to.
  Cabinet * cabinet;

  /// The underlying definition for this collection.
  CollectionDefinition * definition;

  /// Returns the file name format for the given Document.
  QString documentFileNameFormat(const Document * doc) const {
    return documentFileNameFormat(doc->definition);
  };

  /// Returns the file name format for the given DocumentDefinition.
  QString documentFileNameFormat(const DocumentDefinition * def) const;


  /// Returns the display format for the given Document.
  QString documentDisplayFormat(const Document * doc) const{
    return documentDisplayFormat(doc->definition);
  };

  /// Returns the display format for the given DocumentDefinition.
  ///
  /// \todo Here, implement user overrides.
  QString documentDisplayFormat(const DocumentDefinition * def) const {
    return def->displayFormat;
  };

  /// Returns a list of attributes requested for the given
  /// DocumentDefinition
  QHash<QString, AttributeHash::HandledType> attributesRequiredForDocument(const DocumentDefinition * def) const;

  /// Checks whether the given AttributeHash contains all attributes
  /// required for the given document type. Returns the list of
  /// missing attributes
  QStringList missingAttributesForDocument(const AttributeHash & attr, 
                                           const DocumentDefinition * def) const;

  /// All the documents, regardless of their type.
  WatchableList<Document> documents;

  /// Returns the list of Document objects corresponding to each
  /// DocumentDefinition present in this Collection.
  QHash<DocumentDefinition *, QList<Document *> > typeDocuments();

  /// A user-set name, for display purposes only
  QString name;

  /// The base directory for all files stored in this collection.
  QString baseDirectory;

  /// Imports a file as a document of the given type, and returns a
  /// pointer to the newly-created document (whose ownership stays
  /// within the Collection).
  Document * importFile(const QString & doctype, const QString & file);


  virtual SerializationAccessor * serializationAccessor();

  /// The Collection currently being loaded.
  static Collection * collectionBeingSerialized;

  virtual void 	prepareSerializationRead();

  virtual void 	finishedSerializationRead();

  Collection() : cabinet(NULL), definition(NULL) {;};

  /// Returns all the documents in the Collection
  QList<Document *> allDocuments();

  /// Launches the CollectionCode to get all potentially new
  /// documents.
  ///
  /// The attributeHash respresents the credentials of the user.
  ///
  /// \warning When this function returns, not all documents will have
  /// been fetched. Most probably, no Internet access will even have
  /// been performed up to this point ;-)...
  void fetchNewDocumentsForUser(const AttributeHash &user);


  virtual void followLink();

protected:

  /// Checks whether a document with the same file name exists or not.
  /// \todo Possibly provide some caching, and some interaction with
  /// other Collection objects.
  bool fileClashes(const QString &) const;


  /// Checks whether a document with the same file name exists or not.
  bool fileClashes(DocumentDefinition * def,
		   const AttributeHash & attrs) const {
    return fileClashes(attrs.formatString(documentFileNameFormat(def)));
  };

};

/// A group of collections, when it is convenient to group several
/// similar things ? Is it necessary ? In any case, this is not an
/// emergency !
class CollectionFolder : public Serializable {
};
#endif
