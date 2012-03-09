/*
  modelitems.cc: model for transaction lists
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
#include <account.hh>
#include <modelitems.hh>

int ModelItem::myRow() const 
{
  ModelItem * p = parent();
  if(! p)
    return -1;
  return p->childIndex(this);
}

bool ModelItem::setData(int , const QVariant & , int)
{
  return false;
}

Qt::ItemFlags ModelItem::flags(int column) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}


//////////////////////////////////////////////////////////////////////

int FixedChildrenModelItem::childIndex(const ModelItem * child) const
{
  // The const_cast stuff really doesn't sound so great, but I guess
  // there isn't choice...
  return children.indexOf(const_cast<ModelItem*>(child));
}

ModelItem * FixedChildrenModelItem::childAt(int line)
{
  return children.value(line, NULL);
}

FixedChildrenModelItem::~FixedChildrenModelItem()
{
  for(int i = 0; i < children.size(); i++)
    delete children[i];
}


//////////////////////////////////////////////////////////////////////

int LeafModelItem::childIndex(const ModelItem * ) const
{
  return -1;
}

ModelItem * LeafModelItem::childAt(int )
{
  return NULL;
}

int LeafModelItem::columnCount() const 
{
  return 0;
}
