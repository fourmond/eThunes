/*
    document.cc: The Document and DocumentDefinition classes
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
#include <document.hh>
#include <collection.hh>

SerializationAccessor * DocumentDefinition::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("name", 
		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("display-format",
		   new SerializationItemScalar<QString>(&displayFormat));

  return ac;
}

SerializationAccessor * Document::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  return ac;
}



QString Document::displayText()
{
  return attributes.formatString(collection->documentDisplayFormat(this));
}
