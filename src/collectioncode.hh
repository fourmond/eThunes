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

class CollectionDefinition;
class Transaction;

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
/// \todo For attributing a Transaction to a document, there should be
/// a field in the document type giving the name of a Date attribute
/// and a tolerance (a given number of days). For looking, only the
/// transactions within the time range date +- attribute would be
/// considered. (ideally, just a few days for the most common case)
class CollectionCode : public Serializable {
public:

  /// The CollectionDefinition for which this object is the code.
  ///
  /// \todo NULL could be used to denote that it is a Collection-free
  /// bit of code, for, say, downloading bank information from
  /// Internet ?
  CollectionDefinition * definition;

  CollectionCode() : definition(NULL) {;};

  /// Parses the given document and returns the attributes found as a
  /// AttributeHash (or thrown an exception ?).
  ///
  /// The contents are given in the form of an AttributeHash, with
  /// text containing the main text (for instance).
  ///
  /// This function needs to be reimplemented.
  virtual AttributeHash parseDocumentMetaData(const QString & doctype,
					      const AttributeHash & contents) = 0;


  /// Computes a "likeliness" score for the given Transaction.
  ///
  /// A very negative score (to be tweaked later) would mean 'not
  /// implemented' ?
  ///
  /// A small positive score would mean "likely"
  ///
  /// A high positive score would mean "for sure !" There should never
  /// be more than one of those.
  ///
  /// \todo this is interesting for sure to implement in Ruby, but
  /// there probably should be a simple implementation in C++ that
  /// would give a high score for same amount and very close date,
  /// with a bonus with a given string in the transaction name or
  /// memo. Most probably the C++ implementation will be enough for
  /// most cases.
  ///
  /// \todo Write class TransactionMatcher that would do the most
  /// common case for transaction matching: it would be used unless a
  /// specific Ruby function exists.
  virtual int scoreForTransaction(DocumentDefinition * def,
				  const AttributeHash & docMetaData,
				  const AttributeHash & transaction) const;


  /// Overloaded function for convenience. Automatically calls other
  /// one with the relevant data.
  ///
  /// \todo I'm unsure whether this should be here.
  ///
  /// \todo (not here, but well): to store a correspondance
  /// Transaction <-> Document, it should be only necessary to
  /// Serialize the document's name and have a Cabinet function look
  /// at all transactions one the documents have been read and set the
  /// pointers accordingly (on both the Document and the
  /// Transaction). This could take the form of a
  /// dynamically-allocated "static" list of <Transaction *, QString
  /// docname ?>
  int scoreForTransaction(Document * doc, Transaction * tr) const;


  /// Extracts the meta-data from a file
  AttributeHash parseFileMetaData(const QString & doctype,
				  const QString & fileName);


  /// Reads the PDF into an AttributeHash, filling in the contents of
  /// the text, and some meta-data.
  ///
  /// \todo Handle meta-data such as file date/time, file name ? (no
  /// -- why not ? so long as full path isn't included, that shouldn't
  /// leak too much information, especially since most of the time, it
  /// will just be stuff coming from a web server), but file extension
  /// !, and specific PDF information. The former should be done via a
  /// common function (let's think of other kinds of files).
  static AttributeHash readPDF(QString file);


  /// Whether the underlying code supports fetching
  virtual bool canFetch() { return false;};

  /// Looks up new documents in the given website for the given
  /// credentials.
  virtual void fetchNewDocuments(const AttributeHash & credentials,
  				 const QList<AttributeHash> &existingDocuments) = 0;

protected:
};

#endif
