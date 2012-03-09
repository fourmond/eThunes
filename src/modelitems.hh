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

};

/// Base class of items containing a more-or-less fixed number of
/// items.
class FixedChildrenModelItem : public ModelItem {
  Q_OBJECT;

protected:
  QList<ModelItem *> children;

public:
  virtual int childIndex(const ModelItem * child) const;
  virtual ModelItem * childAt(int line);

  virtual ~FixedChildrenModelItem();
};

/// Base class for items that don't have children
class LeafModelItem : public ModelItem {
  Q_OBJECT;

protected:
  QList<ModelItem *> items;

public:
  virtual int childIndex(const ModelItem * child) const;
  virtual ModelItem * childAt(int line);
  virtual int columnCount() const;
};


#endif
