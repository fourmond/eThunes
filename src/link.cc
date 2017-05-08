/*
    linkable.cc: implementation of the Linkable superclass
    Copyright 2010, 2011 by Vincent Fourmond

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
#include <logstream.hh>

#include <exceptions.hh>

SerializationAccessor * Link::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("target-id", &targetID);
  ac->addScalarAttribute("name", &linkName);
  return ac;
}

/// @todo write a template class to hold a type-safe pointer to a
/// Serializable child, that could be serialized. Linkable would build
/// upon it, and provide more functionalities to it. This class would
/// have to have a non-template base class (to store a static array
/// ?).
void Link::setLinkTarget(Linkable * t) 
{
  // target = t;
  if(t)
    targetID = t->ensureHasID();
  else
    targetID = -1;
}

Linkable * Link::linkTarget() const
{
  return Linkable::objectFromID(targetID);
}

void Link::finishedSerializationRead()
{
  // linksToBeFinalized << this;
  // We don't need that anymore ?
}

// @todo : add things here !
void Link::prepareSerializationWrite()
{
  // if(! target)
  //   return;
  // typeName = target->typeName();
  // // linkID = QString();           // Not needed anymore
  // // ensure the ID is correct:
  // if(targetID != target->objectID()) {
  //   throw RuntimeError(QString("ID mismatch %1 != %2 on linkable 0x%3").
  //                      arg(targetID).arg(target->objectID()).
  //                      arg((long)this, 0,16));
  // }
}

QList<Link*> Link::linksToBeFinalized;


/// @todo In principle, I don't need the reference to Cabinet now.
int Link::finalizePendingLinks(Cabinet * cabinet)
{

  /// @todo This should be put back, as dangling links can become
  /// bothersome !

  // int dangling = 0;
  // QList<Linkable *> targets;
  // QHash<Linkable *, bool> done;
  // for(int i = 0; i < linksToBeFinalized.size(); i++) {
  //   Link * link = linksToBeFinalized[i];
  //   if(link->targetID >= 0) {
  //     Serializable * t = Serializable::objectFromID(link->targetID);
  //     Linkable * target = dynamic_cast<Linkable *>(t);
  //     link->setLinkTarget(target);
  //   }
  //   else {                      // old style link
  //     LogStream o;
  //     o << "Found old-style link (ID: " 
  //       << link->linkID << "), converting to new style" << endl;
  //     if(link->typeName == "document")
  //       link->setLinkTarget(cabinet->namedDocument(link->linkID));
  //     else if(link->typeName == "transaction")
  //       link->setLinkTarget(cabinet->wallet.namedTransaction(link->linkID));
  //   }

  //   if(! link->target) {
  //     dangling++;
  //   }

  // }
  // linksToBeFinalized.clear();
  // return dangling;
  return 0;
}

void LinkList::addLink(Linkable * target, const QString & name)
{
  for(int i = 0; i < size(); i++)
    if(value(i).linkTarget() == target)
      return;
  append(Link(target, name));
}

QStringList LinkList::htmlLinkList() const
{
  QStringList ret;
  for(int i=0; i < size(); i++) {
    Linkable * target = value(i).linkTarget();
    if(target)                  // Can be NULL at times
      ret << HTTarget::linkTo(target->publicLinkName(),
                              target);
  }
  return ret;
}

QList<Link *> LinkList::namedLinks(const QString & name)
{
  QList<Link *> retval;
  for(int i = 0; i < size(); i++)
    if(value(i).linkName == name)
      retval.append(& operator[](i));
  return retval;
}

QList<const Link *> LinkList::namedLinks(const QString & name) const
{
  QList<const Link *> retval;
  for(int i = 0; i < size(); i++)
    if(value(i).linkName == name)
      retval.append(& operator[](i));
  return retval;
}
