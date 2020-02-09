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
  categorizableColumns[nativeColumns + CategoryColumn] =
    Categorizable::CategoryColumn;
  categorizableColumns[nativeColumns + TagsColumn] =
    Categorizable::TagsColumn;
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

QString DocumentsModel::absoluteFilePath(const QModelIndex & index) const
{
  QString path = filePath(index);
  return cabinet->baseDirectory().absoluteFilePath(path);
}

int DocumentsModel::columnCount(const QModelIndex &parent) const
{
  return QFileSystemModel::columnCount(parent) + LastColumn;
}

Document * DocumentsModel::modifiableDocument(const QModelIndex & index) const
{
  if(isDir(index))
    return NULL;        // nothing to do here
  QString fn = filePath(index);
  return cabinet->documents.modifiableDocument(fn);
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
    if(categorizableColumns.contains(col) && doc)
      return doc->columnData(categorizableColumns[col], role);
    switch(col - nativeColumns) {
    case TypeColumn: {
      QString n;
      if(doc)
        n = doc->docTypeName();
      if(n.isEmpty()) {
        if(role == Qt::ForegroundRole)
          return QColor(Qt::gray);
        if(role == Qt::DisplayRole)
          return "(no type)";
        return QVariant();
      }
      if(role == Qt::DisplayRole)
        return n;
      return QVariant();
    }
    case LinksColumn:
      if(doc) {
        // QTextStream o(stdout);
        QVariant d = doc->linksData(role);
        // o << "Getting data for doc " << doc << " and role " << role
        //   << "/" << Qt::EditRole 
        //   << " -> " << d.toString() << endl;
        return d;
      }
    case DateColumn:
      if(doc) {
        if(role == Qt::DisplayRole)
          return doc->attributes["date"];
        if(role == Qt::EditRole) {
          QDate dt = doc->attributes["date"].toDate();
          if(dt.isValid())
            return dt;
          return QDate::currentDate();
        }
      }
    case AmountColumn:
      if(doc) {
        if(role == Qt::DisplayRole) {
          if(doc->attributes.contains("amount")) {
            int amount = doc->attributes["amount"].toInt();
            return Transaction::formatAmount(amount);
          }
        }
        if(role == Qt::EditRole) {
          int amount = 0;
          if(doc->attributes.contains("amount"))
            amount = doc->attributes["amount"].toInt();
          return amount;
        }
      }
    default:
      return QVariant();
    }
  }
  return QFileSystemModel::data(index, role);
}

bool DocumentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  int col = index.column();
  if(col >= nativeColumns) {
    if(isDir(index))
      return false;

    QString fn = filePath(index);
    Document * doc = cabinet->documents.modifiableDocument(fn);
    if(categorizableColumns.contains(col)) {
      if(doc->setColumnData(categorizableColumns[col], value, role)) {
        return true;
      }
    }

    switch(col - nativeColumns) {
    case DateColumn:
      if(doc) {
        if(role == Qt::DisplayRole || role == Qt::EditRole) {
          QTextStream o(stdout);
          QVariant v = value;
          o << "Value: " << v.toString() << " -- " << v.type() << endl;
          if(! v.convert(QMetaType::QDate)) {
            QDate s = QDate::fromString(value.toString());
            o << "Date: " << s.toString() << endl;
            v = s;
          }
          doc->attributes["date"] = v;
          return true;
        }
      }
    case AmountColumn:
      if(doc) {
        if(role == Qt::DisplayRole || role == Qt::EditRole) {
          doc->attributes["amount"] = value.toInt();
          return true;
        }
      }
    default:
      return false;
    }
  }
  return QFileSystemModel::setData(index, value, role);
}

Qt::ItemFlags DocumentsModel::flags(const QModelIndex &index) const
{
  int col = index.column();
  if(col >= nativeColumns) {
    if(isDir(index))
      return 0;
    switch(col - nativeColumns) {
    case CategoryColumn:
    case TagsColumn:
    case DateColumn:
    case AmountColumn:
      return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled;
    default:
      return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    }
  }
  return QFileSystemModel::flags(index);
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

