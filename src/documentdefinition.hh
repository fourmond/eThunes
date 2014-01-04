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

class Transaction;

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
public:
  /// The "public" name of this particular kind of documents (bill,
  /// summary, whatever ?)
  QString publicName;

  /// The code-like name of this type; will be used for making up file
  /// names; this is also the hash name.
  QString name;

  /// Public description of the kind of documents, with a little more
  /// details. Could contain links ?
  QString description;

  /// A format string (see AttributeHash::formatString) for public
  /// display; it will be combined with Document::attributes to form
  /// something to be displayed (a label ?)
  ///
  /// It could be overridden by the user in the collection.
  QString displayFormat;

  /// The format of the file name to be used as canonical file name.
  QString fileNameFormat;

  /// @name Transaction-matching attributes
  /// @{

  /// The name of the attribute holding the date relevant for finding
  /// a corresponding Transaction. In principle, the corresponding
  /// Transaction should be on the same day or later.
  QString relevantDate;

  /// Tolerance for looking for the relevant Transaction, in number of
  /// days.
  int transactionDateTolerance;

  /// The name of the attribute holding the amount of the
  /// corresponding Transaction.
  QString relevantAmount;


  /// @}

  /// Which file extensions are "valid" for the given file
  QStringList fileExtensions() const { return QStringList() << "pdf";};

  QString definitionName() const {
    if(publicName.isEmpty())
      return name;
    return publicName;
  };

  /// Builds a document definition from the ruby code.
  DocumentDefinition(VALUE val);

};

#endif
