/**
    \file collectiondefinition.hh
    Collections and their definitions
    Copyright 2010, 2014 by Vincent Fourmond

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


#ifndef __COLLECTIONDEFINITION_HH
#define __COLLECTIONDEFINITION_HH

#include <serializable.hh>
#include <document.hh>
#include <rubymodulecode.hh>

#include <watchablecontainers.hh>
#include <httarget.hh>

class Cabinet;

/// This class represents the definition of a Collection object.
///
///
/// \todo There should be a global hash definition name ->
/// CollectionDefinition, using guarded pointers (QPointer ?)
///
/// \todo Add attributes to identify the author, and possibly
/// copyright, licenses, and so on...
class CollectionDefinition : public Serializable {
protected:
  /// The name of the CollectionDefinition; it must match the base of
  /// file name ! (though it will be automatically set if missing)
  QString name;

  /// The public name of the CollectionDefinition, without
  /// restriction.
  QString publicName;

  /// A public description
  QString description;

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

public:
  /// The definition of the types of documents supported by this
  /// Collection.
  QHash<QString, DocumentDefinition> documentTypes;

  /// Returns a pointer to the named document definition
  DocumentDefinition * documentDefinition(const QString & name)
  {
    if(documentTypes.contains(name))
      return &documentTypes[name];
    return NULL;
  };


  /// Returns the name of the collection definition
  QString getName() const;

  /// Returns the public name
  QString getPublicName() const;

  /// Returns the description
  QString getDescription() const;

  /// A restricted domain: HTTP access from the code of the definition
  /// should not be done outside of this domain (strings prepended
  /// with a . before the domain are fine, though).
  ///
  /// Of course, so long as the Ruby code can setup sockets on its
  /// own, no guarantee can be made with respect to what can be
  /// accessed. But, still !
  ///
  /// @todo This will have to be turned into lists, as for instance
  /// SFR needs two domains.
  QString domain;

  virtual SerializationAccessor * serializationAccessor();

  /// Temporary
  void dumpContents();

  /// Return a hash filter -> name, where filter is a filter suitable
  /// for use with QFileDialog and name is a key of documentTypes.
  QHash<QString,QString> documentFileFilters();

  /// Gets a named CollectionDefinition, loading it from file if
  /// necessary. Returns NULL if not found.
  static CollectionDefinition * namedDefinition(const QString & name);

  /// Browse definitionPath to find all the .def.xml files
  static QStringList availableDefinitions();

  /// Loads the named definition, but without registering it in the
  /// cache. In general, what you want is namedDefinition() instead,
  /// but you might need this for testing purposes.
  static CollectionDefinition * loadWithoutRegistering(const QString &name);


  /// @name Interface to the underlying collection code
  ///
  /// These function interface to the underlying Ruby code of the
  /// collection.
  ///
  /// @{

  /// Parse meta-data from a file
  AttributeHash parseDocumentMetaData(const QString & doctype,
                                      const AttributeHash & contents);

  /// Parses meta-data from a file
  AttributeHash parseFileMetaData(const QString & doctype,
				  const QString & fileName);


  /// Fetches new document from the web
  Fetcher* fetchNewDocuments(const AttributeHash & credentials,
                             const QList<AttributeHash> &existingDocuments,
                             Collection * target);

  int scoreForTransaction(Document * doc, AtomicTransaction * tr) const;

  /// @}
  


protected:

  /// The path in which to seach collections
  ///
  /// \todo Write accessors, in particular to extend the path.
  static QStringList definitionPath;

  /// Loads a named definition from a file named name + ".def.xml" in
  /// the search path.
  static void loadFromFile(const QString &name);

  /// This hash contains the loaded named collection definitions
  ///
  /// \warning This might need to be turned into a pointer to ensure
  /// initialization is done properly, although I don't think it will
  /// lead to problems.
  static QHash<QString, CollectionDefinition *> loadedDefinitions;
};
#endif
