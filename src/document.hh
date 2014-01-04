/**
    \file document.hh
    Documents and their definitions
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


#ifndef __DOCUMENT_HH
#define __DOCUMENT_HH

#include <serializable.hh>
#include <linkable.hh>
#include <managedfile.hh>
#include <attributehash.hh>
#include <documentdefinition.hh>

class Collection;
class CollectionDefinition;

class Transaction;
class DocumentDefinition;

/// A Document is a text-like document (for now, only PDF supported ?)
/// that stores basically anything. It belongs to a Collection, is
/// linked to a real file on the filesystem which can be displayed on
/// demand.
///
/// It necessarily has a date, although the latter can be used only to
/// designate a month, or even a year. It can have an amount attached
/// to it, and possibly a Transaction (how exactly this remains to be
/// seen).
///
/// It has a unique ID in the form of it canonicalFileName().
///
/// Its behavior is to some extent handled by a DocumentDefinition.
///
class Document : public Linkable {
protected:

  /// The list of files attached to this document; the first is the
  /// main one, the other ones are not considered for data collection.
  QList<ManagedFile> attachedFiles;

  /// This function will crash the program if called when
  /// attachedFiles is empty.
  ManagedFile & mainFile() { return attachedFiles[0]; };

public:
  /// The definition of this Document.
  DocumentDefinition * definition;

  /// The Collection to which it belongs
  Collection * collection;

  /// Returns the text used to display the given document.
  QString displayText() const;

  /// The attributes of the document, ie all the information one could
  /// interpret from the file.
  AttributeHash attributes;

  /// Returns an AttributeHash containing, in addition to the
  /// attributes, the name of document type as __type__:
  AttributeHash documentAttributes() const {
    AttributeHash retval = attributes;
    retval["__type__"] = definition->name;
    return retval;
  };

  /// A recent flag, as it will definitely come in useful later on.
  bool recent;

  Document() : definition(NULL), collection(NULL), recent(false) {;}

  virtual void prepareSerializationRead();

  virtual SerializationAccessor * serializationAccessor();

  /// \name File-related functions
  ///
  /// \todo All these functions belong into a specific class handling
  /// all files known to eThunes; in particular, this class would
  /// handle serialization somehow and keep track seriously of the
  /// files.
  ///
  /// @{

  /// Attaches a file to the document; does not move nor copy it.
  ///
  /// \warning This function should not be called when a file is
  /// already attached to this document.
  void setFilePath(const QString & path);

  /// Attaches an auxiliary file to the current document.
  void attachAuxiliaryFile(const QString & path);

  /// The path of the file attached to this Document
  QString filePath(int i = 0) const {
    if(attachedFiles.size() > i)
      return attachedFiles[i].filePath();
    return QString();
  };

  /// The number of ManagedFile attached to this Document.
  int attachmentsNumber() const { return attachedFiles.size();};

  /// Returns the canonical file name for attachement i (defaults to
  /// 0, the main path), relative to Cabinet::baseDirectory,
  /// irrespective of whether the current path points there or not.
  QString canonicalFileName(int i = 0) const;

  /// Returns the absolute canonical file path.
  QString canonicalFilePath(int i = 0) const;

  /// Whether the file is canonical or not.
  bool isFileCanonical(int file) const;

  /// Brings the given file attached to the document into ownership of
  /// the Cabinet, ie copy it to the right place if the current path
  /// is external, or simply renames it if that isn't the case.
  ///
  /// If the file number is -1, it refers to the last item pushed on
  /// the list.
  void bringFileIntoOwnership(int file = 0);

  /// Reparses the file. No safety nets for now.
  void reparseDocument();

  /// @}

  virtual QString uniqueID() const { return canonicalFileName();};

  virtual QString typeName() const { return "document"; };

  virtual QString publicTypeName() const { return QObject::tr("Document"); };

  virtual void followLink();
};

#endif
