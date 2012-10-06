/*
    linkable.cc: implementation of the Linkable superclass
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
#include <linkable.hh>
#include <serializable-templates.hh>


void Linkable::addLinkAttributes(SerializationAccessor * accessor)
{
  accessor->addListAttribute("link", &links);
}

void Linkable::addLink(Linkable * target, const QString & name)
{
  if(!target)
    return;
  links.addLink(target, name);
  target->links.addLink(this, name);
}


int Linkable::hasNamedLinks(const QString & name) const
{
  return links.namedLinks(name).size();
}

Linkable::Linkable()
{
}

Linkable::~Linkable()
{
}
