/*
    documentmodel.cc: implementation of the DocumentsModel class
    Copyright 2017 by Vincent Fourmond

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
#include <document.hh>
#include <cabinet.hh>


DocumentsModel::DocumentsModel(Cabinet * c) :
  cabinet(c)
{
  m_Root = setRootPath(cabinet->baseDirectory().canonicalPath());
  nativeColumns = QFileSystemModel::columnCount(m_Root);
}

QModelIndex DocumentsModel::root() const
{
  return m_Root;
}


QString DocumentsModel::filePath(const QModelIndex & index) const
{
  QString path = QFileSystemModel::filePath(index);
  return cabinet->baseDirectory().relativeFilePath(path);
}

int DocumentsModel::columnCount(const QModelIndex &parent) const
{
  return QFileSystemModel::columnCount(parent) + LastColumn;
}

QVariant DocumentsModel::data(const QModelIndex &index, int role) const
{
  int col = index.column();
  if(role == Qt::DecorationRole && col == 0 && ! isDir(index)) {
    QString fn = QFileSystemModel::filePath(index);
    QMimeType m = mimeDB.mimeTypeForFile(fn);
    return QIcon::fromTheme(m.iconName());
  }
  if(col >= nativeColumns) {
    if(isDir(index))
      return QVariant();        // nothing to do here
    QString fn = filePath(index);
    Document * doc = cabinet->documents.document(fn);
    switch(col - nativeColumns) {
    case TypeColumn: {
      if(! doc ) {
        if(role == Qt::ForegroundRole)
          return QColor(Qt::gray);
        if(role == Qt::DisplayRole)
          return "(no type)";
        return QVariant();
      }
      if(role == Qt::DisplayRole)
        return doc->docTypeName();
      return QVariant();
    }
    case LinksColumn:
      if(doc)
        return Linkable::linksData(doc, role);
    default:
      return QVariant();
    }
  }
  return QFileSystemModel::data(index, role);
}

QVariant DocumentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal) {
    if(section < nativeColumns)
      return QFileSystemModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole) {
      switch(section - nativeColumns) {
      case TypeColumn:
        return tr("Document Type");
      case CategoryColumn:
        return tr("Category");
      case TagsColumn:
        return tr("Tags");
      case LinksColumn:
        return tr("Links");
      case DateColumn:
        return tr("Date");
      case AmountColumn:
        return "Amount";
      default:
        return QVariant();
      }
    }

    return QVariant();
  }
  else
    return QFileSystemModel::headerData(section, orientation, role);
}

