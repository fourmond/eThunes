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

int Linkable::linkIndex(Linkable * target, const QString & name) const
{
  for(int i = 0; i < links.size(); i++) {
    const Link & lnk = links[i];
    if(lnk.linkTarget() == target) {
      if(name.isEmpty() || name == lnk.linkName)
        return i;
    }
  }
  return -1;
}

int Linkable::removeLink(Linkable * target, const QString & name)
{
  int nb = 0;
  int fnd;
  while((fnd = linkIndex(target, name)), fnd >= 0) {
    const Link & lnk = links[fnd];
    QString n = lnk.linkName;
    links.takeAt(fnd);
    nb++;
    target->removeLink(this, n);
  }
  return nb;
}

void Linkable::ensureBidirectionnalLinks()
{
  // This function really is a patch and should never do something,
  // but may help repair mistakes and partially damaged data.

  QTextStream o(stdout);        // Hmmm
  for(const Link & lnk : links) {
    if(lnk.linkTarget()->linkIndex(this, lnk.linkName) < 0) {
      Linkable * tgt = lnk.linkTarget();
      QString tgtn = lnk.linkTarget()->publicLinkName();
      o << "Found dangling link named " << lnk.linkName
        << " from: " << this << "(" << publicLinkName() << ")"
        << " -> " << lnk.linkTarget()
        << "(" << tgtn << ")" << endl;
      // Fixing it
      tgt->addLink(this, lnk.linkName);
    }
  }
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
  watchChild(&links, "links");
}

Linkable::~Linkable()
{
  unregisterSelf();
}

Linkable::Linkable(const Linkable & o) : 
  objectID(o.objectID), links(o.links)
{
  watchChild(&links, "links");
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
  
  while(true) {
    int t = rand();
    if(! targets->contains(t))
      return t;
  }
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
  QMutexLocker l(&targetsLock);
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

QVariant Linkable::linksData(int role)
{
  switch(role) {
  case Qt::EditRole:
    if(links.size())
      return links.htmlLinkList().join(", ");
  default:
    break;
  }
  return QVariant();
}

void Linkable::fillMenuWithLinkableActions(QMenu *menu)
{
  QMenu * sub = new QMenu(QObject::tr("Remove links"));
  for(Link lnk : links) {
    QAction * a = new QAction(lnk.linkTarget()->publicLinkName());
    QObject::connect(a, &QAction::triggered, [lnk, this](bool) {
        removeLink(lnk.linkTarget(), lnk.linkName);
      }
      );
    sub->addAction(a);
  }
  menu->addMenu(sub);
}
