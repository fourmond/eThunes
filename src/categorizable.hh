/**
    \file categorizable.hh
    Base class for objects that can be categorized (using tags and categories)
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


#ifndef __CATEGORIZABLE_HH
#define __CATEGORIZABLE_HH

class Category;
#include <tag.hh>

/// Base class for all the objects that can be the categorized using:
/// @li Category
/// @li Tag
class Categorizable : public virtual Watchable {
protected:

  /// The category
  Category * category;

  /// The list of tags.
  TagList tags;

  /// Adds the serialization of the tags and categories to the given
  /// accessor.
  void addCategoriesSerialization(SerializationAccessor * ac);

  /// Same as setTagList, but using the currently serialized wallet.
  void setTagListPrivate(const QString & str);

  /// Same as setCategoryFromName, but using the currently serialized
  /// wallet.
  void setCategoryFromNamePrivate(const QString & str);

public:


  Categorizable();

  /// Returns the list of tags, formatted as in TagList::toString.
  QString tagString() const {
    return tags.toString();
  };

  /// Sets the tag list, from a comma-separated string. The wallet is
  /// necessary.
  void setTagList(const QString & str, Wallet * wallet = NULL);

  /// Clears the given tag
  void clearTag(Tag * t) {
    tags.clearTag(t);
  }; 

  /// Sets the given tag
  void setTag(Tag * t) {
    tags.setTag(t);
  };

  /// Sets the given named tag
  void setTagFromName(const QString & str, Wallet * wallet = NULL);

  /// Whether the taglist contains the the given tag
  bool hasTag(const Tag * t) const {
    return tags.hasTag(const_cast<Tag*>(t));
  }; 

  /// Sets the category from the given String. If wallet is NULL, it
  /// is taken to be account->wallet, which shouldn't be NULL.
  void setCategoryFromName(const QString & str, Wallet * wallet = NULL);

  /// Returns the full name of the Category, suitable for saving, or
  /// an empty string when there isn't a category.
  QString categoryName() const ;

  /// Returns the category.
  Category * getCategory() const {
    return category;
  };

  /// sets the category.
  void setCategory(Category * c) {
    setAttribute(category, c, "category");
  };

};

#endif
