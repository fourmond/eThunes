/*
    tag.cc: The Tag class
    Copyright 2011 by Vincent Fourmond

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
#include <tag.hh>
#include <wallet.hh>

QString TagList::toString() const
{
  QStringList l;
  for(QList<Tag*>::const_iterator i = constData().begin(); 
      i != constData().end(); i++) 
    l << (*i)->name;
  return l.join(", ");
}

void TagList::fromString(const QString & str, Wallet * wallet)
{
  clear();			// Done with the contents
  QTextStream o(stdout);
  QStringList l = str.split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts);
  for(QStringList::iterator i = l.begin(); i != l.end(); i++) {
    append(wallet->tags.namedTag(*i, true)); // Create if necessary
  }
}


void TagList::setTag(Tag * t)
{
  if(constData().contains(t))
    return;
  append(t);
}

void TagList::clearTag(Tag * t)
{
  int i = constData().indexOf(t);
  if(i >= 0)
    removeAt(i);
}


Tag * TagHash::namedTag(const QString &name, bool create)
{
  iterator i = find(name);
  if(i != end()) {
    return &(i.value());
  }
  if(create) {
    Tag * t = &(*this)[name];
    t->name = name;
    return t;
  }
  return NULL;
}


SerializationAccessor * Tag::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("name",
		   new SerializationItemScalar<QString>(&name, true));
  return ac;
}

Tag::~Tag()
{
}
