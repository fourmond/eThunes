/** 
    \file collectioncode.hh
    Code underlying a Collection
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


#ifndef __COLLECTIONCODE_HH
#define __COLLECTIONCODE_HH

#include <serializable.hh>
#include <document.hh>
#include <attributehash.hh>

/// This class provides means to embed code to do at least some of the
/// following things:
///
/// \li given a PDF file, or rather given its text and
/// meta-information, it should find out all necessary things about
/// the Document, such as its date and its amount if applicable
///
/// \li given a list of transactions and a Document, find out if there
/// is a Transaction likely to correspond to the given Document
///
/// \li It could provide ways to show extract more information about
/// the documents, possibly globally, and to display it somehow ?
///
/// \li It would fetch files from the Internet ! (but this is yet
/// another functionality: if surely data is not dissociable from the
/// way to get it for "usual" documents, for bank accounts it is
/// completely disconnected). The interface for the latter needs to be
/// thought clearly !
///
/// Possibly many derived classes of this pure-virtual class should be
/// used (the Ruby "class" implementation, perhaps a more secure
/// version using only closures ?, Perl/Python ?)
///
/// Currently, this class is more a Java interface than anything else.
class CollectionCode : public Serializable {
public:
  
  /// Parses the given document and returns the attributes found as a
  /// AttributeHash (or thrown an exception ?).
  ///
  /// The contents are given in the form of an AttributeHash, with
  /// text containing the main text (for instance).
  ///
  /// This function needs to be reimplemented.
  virtual AttributeHash parseDocumentMetaData(const QString & doctype,
					      const AttributeHash & contents) = 0;

  /// Extracts the meta-data from a file
  AttributeHash parseFileMetaData(const QString & doctype,
				  const QString & fileName);

protected:
  // Here, functions to transform a PDF file into a QString ?

  /// Reads the PDF into an AttributeHash, filling in the contents of
  /// the text, and some meta-data.
  ///
  /// \todo Handle meta-data such as file date/time, file name ?, and
  /// specific PDF information. The former should be done via a common
  /// function (let's think of other kinds of files).
  static AttributeHash readPDF(QString file);
};

#endif
