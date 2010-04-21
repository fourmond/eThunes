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

/// This class represents the definition of a Collection object. In
/// particular, it will hold:
/// 
/// \li a list of DocumentDefinition, to find out what kind of
/// documents are allowed
///
/// \li the code of a Ruby class... NO ! You can't define anything at
/// $SAFE = 4, and it is not reasonable to run class definition at a
/// lower level.
///
/// This Ruby class will be in charge of many things:
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
class CollectionDefinition : public Serializable {
};

/// This class holds a series of Document objects.
class Collection : public Serializable {
};

/// A group of collections, when it is convenient to group several
/// similar things ?
class CollectionFolder : public Serializable {
};
#endif
