/*
  serializable-templates.cc: a few particular templates
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
#include <serializable.hh>
#include <serializable-templates.hh>

/// Particular implementation that prevents false from being written
///
/// \warning It means that all Serializable using
/// SerializationItemScalar<bool> \b must implement a
/// Serializable::prepareSerializationRead () hook setting the value
/// to false.
template<> QVariant SerializationItemScalar<bool>::valueToVariant()
{
  if(*target)
    return QVariant(*target);
  return QVariant();
}
