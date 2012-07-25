/**
    \file modelitems.hh
    Object-oriented model item
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


#ifndef __MODELITEMS_HH
#define __MODELITEMS_HH

/// The base class of the objects representing items in the view. Each
/// object correspond more-or-less to a QModelIndex, and each valid
/// QModelIndex must hold a pointer to such a ModelItem instance.
///
/// A ModelItem is assumed to take care of a whole \b line of
/// elements (hence the \a column argument to many functions)
///
/// @todo These elements must have a chance to signal changes...
class ModelItem : public QObject {

  Q_OBJECT;
protected:
  ModelItem * _parent;

  /// Setup child tracking (in particular, setup watching it and make sure its
  /// parent pointer points here)
  virtual void trackChild(ModelItem * item);

public:

  ModelItem(ModelItem * p = NULL) : _parent(p) {
  };

  /// A ModelItem instance 
  virtual ~ModelItem() {;};

  virtual ModelItem * parent() const {
    return _parent;
  };

  virtual int rowCount() const = 0;
  virtual int columnCount() const = 0;
  virtual QVariant data(int column, int role) const = 0;
  virtual Qt::ItemFlags flags(int column) const;
  virtual bool setData(int column, const QVariant & value,
		       int role);

  /// Returns the index of the given child, or a negative number if
  /// there isn't.
  virtual int childIndex(const ModelItem * child) const = 0;


  /// Returns the object handling the child at the given location, or
  /// NULL if there isn't any child.
  ///
  /// All ModelItem returned thus \b must be tracked by the instance
  /// returning them and freed when the instance is freed.
  virtual ModelItem * childAt(int line) = 0;

  /// Returns the row relative to the parent, or -1 if no parent
  virtual int myRow() const;

  /// Header data (returns nothing by default);
  virtual QVariant headerData(int column, Qt::Orientation orientation, 
                              int role) const;

  /// Pops up a context menu for the given item. Returns false if no
  /// context menu is available for that item.
  ///
  /// \a pos is to be understood in global coordinates.
  virtual bool popupContextMenu(const QPoint & pos);

signals:
  /// Emitted whenever this or a child item change state.
  void itemChanged(ModelItem * item, int left, int right);

  /// Emitted whenever an item is about to insert (\a nb > 0) or
  /// remove (\a nb < 0) rows (starting at \a start inclusive).
  void rowsWillChange(ModelItem * parent, int start, int nb);

  /// Emitted just after the operation hinted at by rowsWillChange has
  /// been performed.
  ///
  /// @b Note: in principle, the \a item shouldn't be needed.
  void rowsChanged(ModelItem * item);

};

/// Base class of items containing a more-or-less fixed number of
/// items.
/// @todo Add facilities for displaying the list in reverse...
class FixedChildrenModelItem : public ModelItem {
  Q_OBJECT;

protected:
  QList<ModelItem *> children;

  /// Maximum number of columns
  int maxCols;

  /// Child with the maximum number of columns
  int maxColsIndex;

public:

  FixedChildrenModelItem();
  virtual int childIndex(const ModelItem * child) const;
  virtual ModelItem * childAt(int line);
  virtual int rowCount() const;
  virtual int columnCount() const;

  virtual ~FixedChildrenModelItem();

  /// @name Children manipulation functions
  ///
  /// @{
  virtual void insertChild(int index, ModelItem * child);
  virtual void appendChild(ModelItem * child);
  virtual void removeChild(int index, int nb = 1);
  /// @}
};

/// Base class for items that don't have children
class LeafModelItem : public ModelItem {
  Q_OBJECT;

public:
  virtual int childIndex(const ModelItem * child) const;
  virtual ModelItem * childAt(int line);
  virtual int rowCount() const;
};


/// Simple item (more for demonstration purposes than anything else
class TextModelItem : public LeafModelItem {
  Q_OBJECT;

protected:
  QStringList columns;
  bool canEdit;

public:

  TextModelItem(const QStringList & columns);
  TextModelItem(const QString & col, bool ce = true);
  
  virtual int columnCount() const;
  virtual QVariant data(int column, int role) const;

  /// Sets the string value
  void setText(const QString & str, int col = 0);

  virtual Qt::ItemFlags flags(int column) const;
  virtual bool setData(int column, const QVariant & value,
		       int role);
  
};


#endif
