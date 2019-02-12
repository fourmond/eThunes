/**
   \file documentlist.hh
   A list of Document
   Copyright 2017 by Vincent Fourmond

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


#ifndef __DOCUMENTLIST_HH
#define __DOCUMENTLIST_HH

#include <serializable.hh>

class Document;

/// This class collects a series of documents (in general all of them).
///
/// It provides facilities for:
/// @li finding a document by file name
/// @li renaming files and directories
///
/// @todo Make non-owning version. Or use std::unique ?
class DocumentList : public Serializable {
protected:

  /// The storage of the documents
  QHash<QString, Document *> documents;

public:

  DocumentList();
  virtual ~DocumentList();

  /// Returns a pointer to the given document, and create one if
  /// necessary.
  Document * modifiableDocument(const QString & file);

  /// Returns a pointer to the document for the named file, or NULL if
  /// there is no document.
  Document * document(const QString & file) const;

  /// Adds the given Document to the list. Takes ownership of the
  /// Document.
  void addDocument(Document * document);

  /// Returns the list of all the documents
  QList<Document*> allDocuments() const;

  /// Renames the given document.
  bool renameDocument(Document * doc, const QString & newName);

  /// Renames the given path. Makes the right thing about directories.
  void renamePath(const QString & oldPath, const QString & newPath);


protected:

  virtual SerializationAccessor * serializationAccessor() override;

  
};


#endif
