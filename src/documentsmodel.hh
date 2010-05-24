/**
    \file documentsmodel.hh
    An item model providing statistics about categories for a given wallet.
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


#ifndef __DOCUMENTSMODEL_HH
#define __DOCUMENTSMODEL_HH

#include <document.hh>

class DocumentsModel : public QAbstractItemModel {

  Q_OBJECT;

  /// The list of documents.
  QList<Document*> * documents;

public:

  /// Returns the documents corresponding to the index, or NULL for
  /// root/invalid
  Document * indexedDocument(QModelIndex index) const;

  /// The transactions to be displayed.
  DocumentsModel(QList<Document *> * list);


  enum {
    FileColum,
    NameColumn,
    CollectionColumn,
    DoctypeColumn,
    DisplayColumn,
    AttributesColumn,
    LastColumn
  };


  virtual QModelIndex index(int row, int column,
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual int columnCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
			      int role) const ;

public slots:

};

#endif
