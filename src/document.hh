/**
   \file document.hh
   Documents
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


#ifndef __DOCUMENT_HH
#define __DOCUMENT_HH

#include <serializable.hh>
#include <attributehash.hh>
#include <linkable.hh>
#include <categorizable.hh>

class DocType;

/// This class represents a single document.
class Document : public Linkable, public Categorizable {
protected:

  /// The name of the file, referenced from Cabinet::baseDirectory
  QString m_FileName;
  
  /// A user-set comment
  QString comment;

  /// The underlying DocType, optional. Saved as a plain string.
  DocType * m_DocType;

  /// For serialization purposes, and also for saving the document
  /// type name if it is missing (could happen ?)
  QString m_DocTypeName;

public:

  /// The attributes of the document. Can be set manually, or through
  /// automatic detection.
  AttributeHash attributes;

  Document();

  Document(const QString & fileName);

  /// Returns the file name
  QString fileName() const;

  /// Returns the file absolute path
  QString filePath() const;
  

  /// Sets the document type
  void setDocType(DocType * docType);

  /// Returns the current document type
  DocType * docType() const;

  /// Automatically detects the document type.
  void autoDetectDocType();


  /// This small text is provided by the document type, based on the
  /// attributes. Empty when no DocType is available
  QString infoText(); 

  /// Reads the contents of the document, like the contents of the PDF
  /// file, using PDFTools::readPDF, for instance, and returns it as
  /// an AttributeHash. 
  AttributeHash readContents() const;

  virtual QString typeName() const {
    return "document";
  };

  virtual QString publicLinkName() const override;

  virtual void followLink();

  /// Returns the name of the document type (or nothing)
  QString docTypeName() const;

  virtual SerializationAccessor * serializationAccessor() override;

  
  

};


#endif
