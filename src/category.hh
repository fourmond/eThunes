/** 
    \file category.hh
    Class representing a Category to which a Transaction can belong
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


#ifndef __CATEGORY_HH
#define __CATEGORY_HH

#include <serializable.hh>

/// This class represents a Category; these are hierarchical objects
/// to which transactions can belong (but that isn't compulsory). A
/// transaction can only belong to one Category.
class Category : public Serializable {
public:
  /// Sub-categories
  QList<Category> subCategories;

  /// The parent of this category, or NULL if toplevel
  Category * parent;

  /// The name of this category
  QString name;

  /// Returns the full name of this category.
  QString fullName();

  Category();
};

#endif
