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
#include <link.hh>
#include <linkable.hh>
#include <cabinet.hh>

SerializationAccessor * Link::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("id", 
		   new SerializationItemScalar<QString>(&linkID, true));
  ac->addAttribute("type", 
		   new SerializationItemScalar<QString>(&typeName, true));
  return ac;
}

void Link::finishedSerializationRead()
{
  linksToBeFinalized << this;
}

void Link::prepareSerializationWrite()
{
  if(! target) 
    return;
  typeName = target->typeName();
  linkID = target->uniqueID();
}

QList<Link*> Link::linksToBeFinalized;


void Link::finalizePendingLinks(Cabinet * cabinet)
{
  QList<Linkable *> targets;
  for(int i = 0; i < linksToBeFinalized.size(); i++) {
    Link * link = linksToBeFinalized[i];
    // if(link->typeName == "document")
    //   link->target = cabinet->namedDocument(link->linkID);
    // else if(link->typeName == "transaction")
    //   ;
    //   // todo !!
    if(link->target)
      targets << link->target;
  }
  linksToBeFinalized.clear();
  /// \todo Ensure using targets that all the links are reciprocal.
}

void LinkList::addLink(Linkable * target)
{
  for(int i = 0; i < size(); i++)
    if(value(i).target == target)
      return;
  append(Link(target));
}
