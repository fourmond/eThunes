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
#include <tag.hh>
#include <watchablecontainers.hh>

class Category;

/// This class represents a single document.
class Document : public Linkable {
protected:

  /// The name of the file, referenced from Cabinet::baseDirectory
  QString fileName;
  
  /// The category
  Category * category;

  /// The list of tags.
  TagList tags;

  /// A user-set comment
  QString comment;

public:

  Document();


  virtual QString typeName() const {
    return "document";
  };

  virtual QString publicLinkName() const override;

  virtual void followLink();

};


#endif
