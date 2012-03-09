/*
    oomodel.cc: model for transaction lists
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
#include <oomodel.hh>
#include <modelitems.hh>

OOModel::OOModel(ModelItem * r) : root(r) 
{
}

ModelItem * OOModel::item(const QModelIndex & idx, bool giveRoot) const
{
  if(idx.isValid())
    return static_cast<ModelItem*>(idx.internalPointer());
  return (giveRoot ? root : NULL);
}


QModelIndex OOModel::index(int row, int column,
                           const QModelIndex & parent) const
{
  ModelItem * base = item(parent);

  /// @todo bound checking ?
  return createIndex(row, column, base);
}

QModelIndex OOModel::parent(const QModelIndex & index) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return QModelIndex();
  ModelItem * parent = it->parent();
  if(! parent)
    return QModelIndex();
  return createIndex(parent->myRow(), 0, parent);
}

int OOModel::rowCount(const QModelIndex & index) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return 0;
  return it->rowCount();
}

int OOModel::columnCount(const QModelIndex & index) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return 0;
  return it->columnCount();
}

QVariant OOModel::headerData(int section,
				  Qt::Orientation /*orientation*/,
				  int role) const
{
  /// @todo ?
  return QVariant();
}


QVariant OOModel::data(const QModelIndex& index, int role) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return QVariant();
  return it->data(index.column(), role);
}

Qt::ItemFlags OOModel::flags(const QModelIndex & index) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return 0;
  return it->flags(index.column());
}

bool OOModel::setData(const QModelIndex & index, const QVariant & value,
			   int role)
{
  ModelItem * it = item(index, false);
  if(! it)
    return false;
  return it->setData(index.column(), value, role);
}



