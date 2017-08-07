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


// The lock for modifying the targets.
static QMutex targetsLock;

QHash<int, QSet<Linkable * > > * Linkable::targets = NULL;

Linkable::Linkable() : objectID(-1)
{
  // No need to register
}

Linkable::~Linkable()
{
  unregisterSelf();
}

Linkable::Linkable(const Linkable & o) : 
  objectID(o.objectID), links(o.links)
{
  registerSelf();
}

Linkable & Linkable::operator=(const Linkable & o)
{
  objectID = o.objectID;
  links = o.links;
  registerSelf();
  return *this;
}

int Linkable::ensureHasID() const
{
  if(objectID < 0) {
    QMutexLocker l(&targetsLock);
    const_cast<Linkable *>(this)->objectID = getFreeID();
    registerSelf();
  }
  return objectID;
}

int Linkable::getFreeID()
{
  if(! targets)
    targets = new QHash<int, QSet<Linkable * > >;
  
  for(int i = 0; i <= targets->size(); i++)
    if(! targets->contains(i))
      return i;
  Q_ASSERT(false);              // This should never be reached
  return targets->size();
}

void Linkable::registerSelf() const
{
  if(objectID < 0)
    return;                     // Nothing to do

  if(! targets)
    targets = new QHash<int, QSet<Linkable * > >;
  if(! targets->contains(objectID))
    (*targets)[objectID] = QSet<Linkable *>();
  (*targets)[objectID].insert(const_cast<Linkable*>(this));
}

void Linkable::unregisterSelf() const
{
  if(objectID < 0)
    return;
  (*targets)[objectID].remove(const_cast<Linkable*>(this));
}


QString Linkable::objectIDGet() const
{
  if(objectID >= 0) 
    return QString("%1").arg(objectID);
  return QString();
}

void Linkable::objectIDSet(const QString & g)
{
  int id = g.toInt();
  if(id < 0) {
    objectID = -1;
    return ;
  }
  objectID = id;
  registerSelf();
}

void Linkable::addIDSerialization(SerializationAccessor * accs)
{
  accs->addAttribute("ID",
                     new SerializationItemAccessors<Linkable>
                     (this, &Linkable::objectIDSet, 
                      &Linkable::objectIDGet, 
                      true));
}

Linkable * Linkable::objectFromID(int id)
{
  if(! targets)
    return NULL;
  if(! targets->contains(id))
    return NULL;

  const QSet<Linkable *> & s = (*targets)[id];
  if(s.size() == 0)
    return NULL;


  if(s.size() == 1)
    return *(s.begin());

  // We only output a warning for the time being

  {
    QTextStream o(stdout);
    o << "WARNING: duplicate entry for object ID " << id << endl;
  }
  return *(s.begin());
}

QVariant Linkable::linksData(Linkable * lnk, int role)
{
  switch(role) {
  case Qt::EditRole:
    if(lnk->links.size())
      return lnk->links.htmlLinkList().join(", ");
  default:
    break;
  }
  return QVariant();
}
