/*
    category.cc: The Category class
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

#include <headers.hh>
#include <category.hh>

Category::Category()
{
  parent = 0;
}

#define CategorySeparator "::"

QString Category::fullName()
{
  if(! parent)
    return name;
  return parent->fullName() + CategorySeparator + name;
}

/// \todo We are seriously leaking memory here. Fix that up by using a
/// proper copy constructor, rather than the default one.
Category::~Category()
{
  // if(subCategories)		// But it really should always be the
  // 				// case.
  //   delete subCategories;
}

SerializationAccessor * Category::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("name", 
		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("category", 
		   new SerializationQHash<Category>(&subCategories));
  return ac;
}

Category * CategoryHash::namedSubCategory(const QString &name, bool create)
{

  int index;
  if(name.isEmpty())
    return NULL;
  if(index = name.indexOf(CategorySeparator), index >= 0) {
    QString root = name.left(index);
    QString subPath = name.mid(index + QString(CategorySeparator).size());
    Category * sub = namedSubCategory(root, create);
    if(sub)
      return sub->namedSubCategory(subPath, create);
    else
      return NULL;
  }
  else {
    // Plain key
    if(contains(name))
      return &operator[](name);
    if(!create)
      return NULL;

    insert(name, Category());
    value(name).subCategories.dumpContents(" -> ");
    operator[](name).name = name;
    return &operator[](name);
  }
}

void CategoryHash::dumpContents(QString prefix) const 
{
  QTextStream o(stdout);
  QStringList l = keys();
  for(int i = 0; i < l.size(); i++) {
    o << prefix << l[i] << " (" << &operator[](l[i]) << ")" << endl;
    operator[](l[i]).subCategories.dumpContents(prefix + "  ");
  }
}

Category * Category::namedSubCategory(const QString &name, bool create)
{
  Category * sub = subCategories.namedSubCategory(name, create);
  if(sub)
    sub->parent = this;
  return sub;
}

bool Category::isChildOf(Category * category)
{
  Category * c = this;
  while(c->parent) {
    if(c->parent == category)
      return true;
    c = c->parent;
  }
  return false;
}
