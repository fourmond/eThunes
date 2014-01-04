/*
    documentdefinition.cc: The Document and DocumentDefinition classes
    Copyright 2010, 2014 by Vincent Fourmond

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
#include <documentdefinition.hh>
#include <collection.hh>
#include <cabinet.hh>

#include <ruby-utils.hh>

DocumentDefinition::DocumentDefinition(VALUE val) 
  : rubyObject(val)
{
}

QString DocumentDefinition::getName() const
{
  VALUE v = rb_iv_get(rubyObject, "@name");
  return VALUE2QSTRING(v);
}

QString DocumentDefinition::getPublicName() const
{
  VALUE v = rb_iv_get(rubyObject, "@public_name");
  return VALUE2QSTRING(v);
}

QString DocumentDefinition::getDisplayFormat() const
{
  VALUE v = rb_iv_get(rubyObject, "@display");
  return VALUE2QSTRING(v);
}

QString DocumentDefinition::getFileNameFormat() const
{
  VALUE v = rb_iv_get(rubyObject, "@format");
  return VALUE2QSTRING(v);
}

QString DocumentDefinition::definitionName() const
{
  QString pn = getPublicName();
  if(pn.isEmpty())
    return getName();
  return pn;
}

AttributeHash DocumentDefinition::parseDocumentMetaData(const AttributeHash & contents) const
{
  AttributeHash target;
  VALUE hash = contents.toRuby(); // unsafe ?
  VALUE result = Ruby::safeFuncall(rubyObject, Ruby::parseID, 1, hash);
  target.setFromRuby(result); // unsafe ?
  return target;
}