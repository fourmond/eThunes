/*
    serializable.cc: Infrastructure for data serialization
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

#include <serializable.hh>

QList<Serializable *> Serializable::registeredObjects;


Serializable::Serializable() : serializableID(-1) 
{
}

SerializationAccessor * Serializable::serializationAccessor()
{
  return new SerializationAccessor(this);
}

void Serializable::ensureHasID() const
{
  if(serializableID >= 0)
    return;                     // Nothing to do
  else {
    /// @todo Not MT-safe. (but there shouldn't be needs for that ?)
    serializableID = registeredObjects.size();
    registeredObjects.append(const_cast<Serializable*>(this));
  }
}

int Serializable::objectID() const 
{
  ensureHasID();
  return serializableID;
}

void Serializable::writeXML(const QString & name, QXmlStreamWriter * writer)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->writeXML(name, writer);
  delete ac;
}

void Serializable::readXML(QXmlStreamReader * reader)
{
  SerializationAccessor * ac = serializationAccessor();
  ac->readXML(reader);
  delete ac;
}

Serializable::~Serializable()
{
  if(serializableID >= 0) 
    registeredObjects[serializableID] = NULL;
}

/// @todo A compaction function could be written to ensure that all
/// elements in registeredObjects are not NULL. This would help keep
/// the ID number relatively small and manageable, and could be called
/// right after Link::finalizePendingLinks().
Serializable * Serializable::objectFromID(int id)
{
  if(id < 0)
    return NULL;
  return registeredObjects[id];
}


QString Serializable::serializableIDGet() const
{
  if(serializableID >= 0) 
    return QString("%1").arg(serializableID);
  return QString();
}

void Serializable::serializableIDSet(const QString & g)
{
  int id = g.toInt();
  if(id < 0) {
    serializableID = -1;
    return ;
  }
  if(registeredObjects.size() <= id) {
    for(int s = registeredObjects.size(); s <= id; s++)
      registeredObjects.append(NULL);
  }
  Serializable * ptr = registeredObjects[id];
  if(ptr && ptr != this) {
    throw QString("Conflict !"); // Probably a little too brutal...
  }
  registeredObjects[id] = this;
  serializableID = id;
}

void Serializable::addIDSerialization(SerializationAccessor * accs)
{
  accs->addAttribute("ID",
                     new SerializationItemAccessors<Serializable>
                     (this, &Serializable::serializableIDSet, 
                      &Serializable::serializableIDGet, 
                      true));
}
