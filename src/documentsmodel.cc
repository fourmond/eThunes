/*
    documentsmodel.cc: model for displaying documents information
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
#include <documentsmodel.hh>
#include <collection.hh>

DocumentsModel::DocumentsModel(QList<Document *> * list) :
  documents(list)
{
}

Document * DocumentsModel::indexedDocument(QModelIndex index) const
{
  return static_cast<Document *>(index.internalPointer());
}

QModelIndex DocumentsModel::index(int row, int column,
				 const QModelIndex & parent) const
{
  if(parent.isValid()) {
    if(indexedDocument(parent))
      return QModelIndex();
    if(row >= documents->count())
      return QModelIndex();
    return createIndex(row, column,
		       (*documents)[row]);
  }
  else
    return createIndex(row, column, (void *)NULL);
}

QModelIndex DocumentsModel::parent(const QModelIndex & index) const
{
  if(index.isValid()) {
    Document * c = indexedDocument(index);
    if(c)
      return createIndex(0,0, (void*) NULL);
    else
      return QModelIndex();
  }
  else
    return QModelIndex();
}

int DocumentsModel::rowCount(const QModelIndex & index) const
{
  if(index.isValid()) {
    if(indexedDocument(index))
      return 0;
    return documents->count();
  }
  return 0;
}

int DocumentsModel::columnCount(const QModelIndex & /*index*/) const
{
  return LastColumn;
}

QVariant DocumentsModel::headerData(int section,
				  Qt::Orientation /*orientation*/,
				  int role) const
{
  if(role == Qt::DisplayRole) {
    switch(section) {
    case NameColumn: return QVariant(tr("Canonical name"));
    case CollectionColumn: return QVariant(tr("Collection"));
    case DoctypeColumn: return QVariant(tr("Document type"));
    case DisplayColumn: return QVariant(tr("Description"));
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


QVariant DocumentsModel::data(const QModelIndex& index, int role) const
{
  const Document *doc = indexedDocument(index);
  if(! doc)
    return QVariant();
  switch(role) {
  case Qt::DisplayRole:
    switch(index.column()) {
    case NameColumn:
      return doc->canonicalFileName();
    case CollectionColumn:
      return doc->collection->name;
    case DoctypeColumn:
      return doc->definition->definitionName();
    case DisplayColumn:
      return doc->displayText();
    }
    break;
  default:
    return QVariant();
  }
  return QVariant();
}



