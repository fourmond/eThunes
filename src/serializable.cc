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

void SerializationItem::setFromString(const QString & v)
{
  setFromVariant(v);
}

QString SerializationItem::valueToString()
{
  return valueToVariant().toString();
}


SerializationAccessor * Serializable::serializationAccessor()
{
  return new SerializationAccessor;
}

SerializationAccessor::~SerializationAccessor()
{
  QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  while (i.hasNext()) {
     i.next();
     delete i.value();
  }
  simpleAttributes.clear();
  
}

/// \todo raise exception when already exists.
void SerializationAccessor::addSimpleAttribute(QString name, 
						  SerializationItem * ser)
{
  simpleAttributes[name] = ser;
}

void SerializationAccessor::dumpValues()
{
  QTextStream o(stdout);
  o << "Dumping values for object: " << target << endl;
  QHashIterator<QString, SerializationItem *> i(simpleAttributes);
  while (i.hasNext()) {
    i.next();
    o << i.key() << " : " << i.value()->valueToString() << endl;
  }
}