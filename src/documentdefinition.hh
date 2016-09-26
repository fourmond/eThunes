/**
    \file documentdefinition.hh
    Documents and their definitions
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


#ifndef __DOCUMENTDEFINITION_HH
#define __DOCUMENTDEFINITION_HH

#include <attributehash.hh>

class Collection;
class CollectionDefinition;

class AtomicTransaction;
class Document;

/// This class defines the kind of Document objects one can find in a
/// given CollectionDefinition.
///
/// \todo Here should be implemented some day a file type mechanism;
/// this class probably would be the perfect place to do so.
///
/// Ideally, this class should be able to take a PDF document and
/// return the appropriate Document object with all the
/// meta-information correct.
class DocumentDefinition  {
protected:
  /// The underlying Ruby object
  VALUE rubyObject;
public:
  /// @name Transaction-matching capacities
  /// @{

  /// Returns the score for the match between the given transaction
  /// and the given document.
  int scoreForTransaction(Document * doc, AtomicTransaction * tr) const;

  /// Returns the relevant date range
  QPair<QDate, QDate> relevantDateRange(Document * doc) const;

  /// @}


  /// @name Ruby-given attributes
  ///
  /// @{

  /// Name of the document definition
  QString getName() const;

  /// Public name
  QString getPublicName() const;

  /// Display format
  QString getDisplayFormat() const;

  /// File name format
  QString getFileNameFormat() const;

  /// Which file extensions are "valid" for the given file
  QStringList fileExtensions() const;


  /// Returns the list of required attributes
  QStringList getAttributes() const;

  /// The name of the definition (public name, but falls back to other
  /// name should this be empty)
  QString definitionName() const;

  /// @}

  /// Builds a document definition from the ruby code.
  DocumentDefinition(VALUE val);


  /// Parses the given document
  AttributeHash parseDocumentMetaData(const AttributeHash & contents) const;


};

#endif
