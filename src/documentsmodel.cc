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
#include <cabinet.hh>


DocumentsModel::DocumentsModel(Cabinet * c) :
  cabinet(c)
{
  m_Root = setRootPath(cabinet->baseDirectory().canonicalPath());
}

QModelIndex DocumentsModel::root() const
{
  return m_Root;
}

int DocumentsModel::columnCount(const QModelIndex &parent) const
{
  return QFileSystemModel::columnCount(parent) + Last;
}

QVariant DocumentsModel::data(const QModelIndex &index, int role) const
{
  int col = index.column();
  int cc = QFileSystemModel::columnCount(index);
  if(role == Qt::DecorationRole && col == 0 && ! isDir(index)) {
    QString fn = filePath(index);
    QMimeType m = mimeDB.mimeTypeForFile(fn);
    return QIcon::fromTheme(m.iconName());
  }
  return QFileSystemModel::data(index, role);
}
 
