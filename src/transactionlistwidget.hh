/**
    \file transactionlistwidget.hh
    A page displaying more information about an account
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

#ifndef __TRANSACTIONLISTWIDGET_HH
#define __TRANSACTIONLISTWIDGET_HH

#include <oomodel.hh>
#include <accountmodel.hh>

class TransactionListItem;

/// This Widget is a thin wrapper around a QTreeView with an
/// AccoutModel as model.
class TransactionListWidget : public QWidget {

  Q_OBJECT;
protected:
  /// \todo Find a way to disable columns on demand.
  void setupFrame();

  void setupTreeView();

  /// Fills the menu with the given Category and its children.
  void fillMenuWithCategory(QMenu * menu, Category * category);

  /// Fills the menu with the given CategoryHash
  void fillMenuWithCategoryHash(QMenu * menu, CategoryHash * ch);

  /// Fills the menu with the given Category and its children.
  void fillMenuWithTags(QMenu * menu, TagHash * tags, 
			const QString &action);

public:

  TransactionListWidget(QWidget * parent = 0);
  TransactionListWidget(TransactionList *transactions,
			QWidget * parent = 0);
  TransactionListWidget(TransactionPtrList *transactions,
			QWidget * parent = 0);

  void showTransactions(TransactionList *transactions);
  void showTransactions(TransactionPtrList *transactions);


  /// The model holding the data
  AccountModel * model;

  /// The root item
  TransactionListItem * rootItem;

  /// And the widget to actually display stuff
  QTreeView * view;

  ~TransactionListWidget();

  /// Returns the natural width of the widget, ie the one it would
  /// really wish to have to accomodate for all the columns.
  int naturalWidth() const;

  virtual QSize sizeHint() const;

  /// Returns a list of the currently selected transactions.
  TransactionPtrList selectedTransactions() const;

  /// Returns the current transaction
  AtomicTransaction *  currentTransaction() const;

  /// Returns the Wallet associated
  Wallet * wallet() const;

signals:


public slots:

  /// Shows the given transaction
  void showTransaction(Transaction * transaction);

  /// Fires up a context menu for the given item.
  void fireUpContextMenu(const QPoint & pos);

  /// Shows the balance column
  void showBalance();

  /// Hides the balance column
  void hideBalance();

protected slots:

  /// To be fired up from the context menu.
  void contextMenuActionFired(QAction * action);

  void onItemExpanded(const QModelIndex & idx);

  /// This slot ensures that all Link "editors" are created where
  /// needed
  void ensureEditorsOn(const QModelIndex & tl, const QModelIndex & bl);

};

#endif
