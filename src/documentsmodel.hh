/**
   \file documentmodel.hh
   A subclass of QFileSystemModel handling Document meta-data
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


#ifndef __DOCUMENTSMODEL_HH
#define __DOCUMENTSMODEL_HH

class Document;
class Cabinet;


/// This model handle the display of all the items
class DocumentsModel : public QFileSystemModel {
protected:

  /// The cabinet we're looking at
  Cabinet * cabinet;

  QModelIndex m_Root;


  QMimeDatabase mimeDB;

  /// The number of columns in the 
  int nativeColumns;

public:

  DocumentsModel(Cabinet * cabinet);

  QModelIndex root() const;

  /// Returns the file path, <b>relative to the Cabinet base directory</b>.
  QString filePath(const QModelIndex & index) const;


  enum AdditionalColumns {
    TypeColumn,
    CategoryColumn,
    TagsColumn,
    LinksColumn,
    DateColumn,
    AmountColumn,
    LastColumn
  };


  // Reimplementation of superclass functions
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};


#endif
