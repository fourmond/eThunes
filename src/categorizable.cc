/*
    categorizable.cc: implementation of the Categorizable superclass
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

#include <headers.hh>
#include <categorizable.hh>
#include <cabinet.hh>
#include <wallet.hh>
#include <serializable-templates.hh>

Categorizable::Categorizable() :
  category(NULL)
{
  watchChild(&tags, "tags");
}

void Categorizable::addCategoriesSerialization(SerializationAccessor * ac)
{
  ac->addAccessorsAttribute("category",this, 
                            &Categorizable::setCategoryFromNamePrivate, 
                            &Categorizable::categoryName);
  ac->addAccessorsAttribute("tags",this, 
                            &Categorizable::setTagListPrivate, 
                            &Categorizable::tagString);
}

QString Categorizable::categoryName() const
{
  if(category)
    return category->fullName();
  return QString();
}

void Categorizable::setCategoryFromName(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  setCategory(w->categories.namedSubCategory(str, true));
}

void Categorizable::setTagFromName(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  Tag * t = w->tags.namedTag(str, true);
  setTag(t);
}


void Categorizable::setTagListPrivate(const QString & str) 
{
  setTagList(str, Wallet::walletCurrentlyRead);
}

void Categorizable::setCategoryFromNamePrivate(const QString & str) 
{
  setCategoryFromName(str, Wallet::walletCurrentlyRead);
}

void Categorizable::setTagList(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  tags.fromString(str, w);
}
