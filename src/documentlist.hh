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

#include <document.hh>


/// This class collects a series of documents (in general all of them).
///
/// It provides facilities for:
/// @li finding a document by file name
/// @li rename files and directories
class DocumentList : public Serializable {
protected:

  
public:

  DocumentList();



};


#endif
