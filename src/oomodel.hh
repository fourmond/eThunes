/**
    \file oomodel.hh
    A fully object-oriented item model implementation
    Copyright 2012 by Vincent Fourmond

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



#ifndef __OOMODEL_HH
#define __OOMODEL_HH

class ModelItem;

/// This class marshalls a collection of ModelItem objects (based on
/// one root object) into the item-model-view paradigm.
class OOModel : public QAbstractItemModel {

  Q_OBJECT;

protected:
  
  /// The root item of the model.
  ModelItem * root;

  /// Returns the ModelItem corresponding to this index, or the root
  /// item if the index is invalid.
  ///
  /// @todo Have a version that returns NULL for an invalid index ?
  ModelItem * item(const QModelIndex & idx, bool giveRoot = true) const;

  QModelIndex indexForItem(ModelItem * item, int col = 0) const;

public:

  /// Creates a OOModel based on the given root. Takes ownership of
  /// the root !
  OOModel(ModelItem * r);

  virtual QModelIndex index(int row, int column,
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual int columnCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
			      int role) const ;

  virtual Qt::ItemFlags flags(const QModelIndex & index) const;

  virtual bool setData(const QModelIndex & index, const QVariant & value,
		       int role = Qt::EditRole);

  
  QModelIndex rootIndex() const;

  /// Sets a new root (and deletes the previous one).
  void setRoot(ModelItem * newRoot);

  virtual ~OOModel();

  ModelItem * currentRoot() const {
    return root;
  };

protected:

  /// Setups the root.
  void setupRoot(ModelItem * r);

protected slots:
  void onItemChanged(ModelItem * item, int left, int right);
  void onRowsAboutToChange(ModelItem * item, int start, int nb);
  void onRowsChanged(ModelItem * item);

protected:
  bool lastChangeIsInsertion;
  
};


#endif
