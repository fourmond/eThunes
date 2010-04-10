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

class Category;

/// A hash of Category, knowing that they sho
class CategoryHash : public QHash<QString, Category> {
public:
  /// Returns a pointer to the named (sub) category, or NULL if it
  /// does not exist.
  ///
  /// On the other hand, if create is true, it is created even if it
  /// does not exist.
  ///
  /// If name is empty, returns NULL
  Category * namedSubCategory(const QString &name, bool create = false);

  /// Dumps the contents of the hash
  void dumpContents(QString prefix = "") const;

  /// Returns the number of categories, including their subcategories
  int categoryCount() const;
};


/// This class represents a Category; these are hierarchical objects
/// to which transactions can belong (but that isn't compulsory). A
/// transaction can only belong to one Category.
class Category : public Serializable {
public:
  /// Sub-categories
  CategoryHash  subCategories;

  /// The parent of this category, or NULL if toplevel
  Category * parent;

  /// The name of this category
  QString name;

  /// Returns the full name of this category.
  QString fullName();

  /// The color of the caterogy. invalid means "unspecified"
  QColor color;

  Category();

  /// Returns a pointer to the named (sub) category, or NULL if it
  /// does not exist.
  ///
  /// On the other hand, if create is true, it is created even if it
  /// does not exist, and its the parent is set to this.
  Category * namedSubCategory(const QString &name, bool create = false);

  /// Whether this Category is a child of the given Category.
  bool isChildOf(const Category * category);


  virtual SerializationAccessor * serializationAccessor();

  virtual ~Category();

  /// Returns the number of categories, including this and the
  /// subcategories
  int count() const;

  /// Returns the color of the category, or the one of its parent if
  /// invalid.
  QColor categoryColor();
};


#endif
