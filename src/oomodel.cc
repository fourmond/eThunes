/*
    oomodel.cc: A fully object-oriented item model implementation
    Copyright 2010, 2011, 2012 by Vincent Fourmond

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

OOModel::OOModel(ModelItem * r) : 
  lastChangeIsInsertion(false) // not that it matters
{
  setupRoot(r);
}

ModelItem * OOModel::item(const QModelIndex & idx, bool giveRoot) const
{
  if(idx.isValid())
    return static_cast<ModelItem*>(idx.internalPointer());
  return (giveRoot ? root : NULL);
}

QModelIndex OOModel::indexForItem(ModelItem * it, int col) const
{
  if(! it)
    return QModelIndex();
  return createIndex(it->myRow(), col, it);
}

QModelIndex OOModel::index(int row, int column,
                           const QModelIndex & parent) const
{
  ModelItem * base = item(parent);
  if(! base)
    return QModelIndex();       // Invalid
  ModelItem * it = base->childAt(row);
  if(! it)
    return QModelIndex();
  return createIndex(row, column, it);
}

QModelIndex OOModel::parent(const QModelIndex & index) const
{
  ModelItem * it = item(index, false);
  if(! it)
    return QModelIndex();
  return indexForItem(it->parent());
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
                             Qt::Orientation orientation,
                             int role) const
{
  return root->headerData(section, orientation, role);
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

QModelIndex OOModel::rootIndex() const
{
  return createIndex(0, 0, root);
}

void OOModel::setRoot(ModelItem * newRoot)
{
  beginResetModel();
  delete root;
  setupRoot(newRoot);
  endResetModel();
}

OOModel::~OOModel()
{
  delete root;
}

void OOModel::setupRoot(ModelItem * nr)
{
  root = nr;
  connect(root, SIGNAL(itemChanged(ModelItem *, int, int)),
          SLOT(onItemChanged(ModelItem *, int, int)));
  connect(root, SIGNAL(rowsChanged(ModelItem *)),
          SLOT(onRowsChanged(ModelItem *)));
  connect(root, SIGNAL(rowsWillChange(ModelItem *, int, int)),
          SLOT(onRowsAboutToChange(ModelItem *, int, int)));

}
          
          
void OOModel::onItemChanged(ModelItem * item, int left, int right)
{
  emit(dataChanged(indexForItem(item, left), indexForItem(item, right)));
}

void OOModel::onRowsAboutToChange(ModelItem * item, int start, int nb)
{
  if(nb == 0)
    return;
  QModelIndex idx = indexForItem(item);
  int end = start + abs(nb) - 1;
  QTextStream o(stdout);
  o << "Rows about to change: " << item 
    << " - " << start << " -- " << nb << " -- " << end <<  endl;
  if(nb > 0) {
    beginInsertRows(idx, start, end);
    lastChangeIsInsertion = true;
  }
  else {
    beginRemoveRows(idx, start, end);
    lastChangeIsInsertion = false;
  }
}

void OOModel::onRowsChanged(ModelItem * item)
{
  QTextStream o(stdout);
  o << "Rows finished to change: " << item  << endl;
  if(lastChangeIsInsertion)
    endInsertRows();
  else
    endRemoveRows();
}
          
