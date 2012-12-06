/**
    \file account-model.hh
    An item model providing transaction data
    Copyright 2010, 2011, 2012 by Vincent Fourmond

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


#ifndef __ACCOUNTMODEL_HH
#define __ACCOUNTMODEL_HH

#include <transaction.hh>
#include <transactionlists.hh>
#include <modelitems.hh>
#include <oomodel.hh>


/// This item represents a single \b leaf transaction
class LeafTransactionItem : public LeafModelItem {
  Q_OBJECT;

  AtomicTransaction * transaction;

public:
  LeafTransactionItem(AtomicTransaction * t);
  virtual int columnCount() const;
  virtual QVariant data(int column, int role) const;
  virtual Qt::ItemFlags flags(int column) const;
  virtual bool setData(int column, const QVariant & value,
		       int role);

  /// Changes the underlying transaction pointer.
  void changeTransaction(AtomicTransaction * newt);

  /// Returns the transaction 
  AtomicTransaction * getTransaction() const { return transaction; };

protected slots:
  void transactionChanged();

};

/// This represents a full transaction along with its subtransactions
/// when applicable.
class FullTransactionItem : public FixedChildrenModelItem {
  Q_OBJECT;

  Transaction * transaction;

protected:

  virtual int rootColumns() const;


public:
  FullTransactionItem(Transaction * transaction);

  virtual QVariant data(int column, int role) const;  
  virtual QVariant headerData(int column, Qt::Orientation orientation, 
                              int role) const;  

  /// Changes the underlying transaction pointer.
  void changeTransaction(Transaction * newt);

  /// Returns the transaction 
  Transaction * getTransaction() const { return transaction; };


protected slots:
  void onAttributeChanged(const Watchdog * wd, const QString &name);
  void onObjectInserted(const Watchdog * wd, int at, int nb);
  void onObjectRemoved(const Watchdog * wd, int at, int nb);

  void transactionChanged();
};


/// Base class for the TransactionListItem template classes
class BaseTransactionListItem : public FixedChildrenModelItem {
  Q_OBJECT;

public:
  virtual QVariant data(int column, int role) const;  
  virtual QVariant headerData(int column, Qt::Orientation orientation, 
                              int role) const;  

  /// Returns the account linked to the transactions (ie the first one
  /// found)
  virtual Account * account() const = 0;

  /// Looks for a given transaction, and returns the corresponding item
  virtual ModelItem * findTransaction(const AtomicTransaction * t) = 0;
  virtual ModelItem * findTransaction(const Transaction * t) = 0;

protected slots:
  void onAttributeChanged(const Watchdog * wd, const QString &name);
  void onObjectInserted(const Watchdog * wd, int at, int nb);
  void onObjectRemoved(const Watchdog * wd, int at, int nb);
protected:
  virtual void effAttributeChanged(const Watchdog * wd, const QString &name) = 0;
  virtual void effObjectInserted(const Watchdog * wd, int at, int nb) = 0;
  virtual void effObjectRemoved(const Watchdog * wd, int at, int nb) = 0;
};


/// Class in charge of organising the data of a list of Transaction
/// objects into something viewable with a Tree or Table view.
///
/// \todo Ideas:
/// \li color background (or whathever) by month (possibly moving)
/// \li color balance/amount by credit/debit
/// \li color lines/specific parts according to the category
/// More ideas ?
///
/// \todo Tanya says it would be much nicer if the transactions were
/// grouped by month in a tree-like fashion. This is easy, but maybe a
/// little long ?
class AccountModel : public OOModel {

  Q_OBJECT;

  /// List of transactions:
  TransactionList * transactions;

  /// Or this way:
  TransactionPtrList * transactionsPtr;

  BaseTransactionListItem * rootItem() const;

protected:

  /// Static cache for icons
  static QHash<QString, QIcon> statusIcons;


public:

  /// Returns a small icon (16x16) representing a given status
  static const QIcon & statusIcon(const QString & status);

  AccountModel(TransactionList * transactions);
  AccountModel(TransactionPtrList * transactions);

  /// Setup the model to be looking at the given list
  void setList(TransactionList * transactions);
  void setList(TransactionPtrList * transactions);

  /// Returns the transaction corresponding to the index, or NULL if
  /// invalid or root.
  ///
  /// @todo Replace with AtomicTransaction !
  Transaction * indexedTransaction(QModelIndex index) const;


  /// @todo The order of the columns should probably be made
  /// customizable.
  enum {
    RecentColumn,		/// Whether the transaction is new or not ?
    DateColumn,
    AmountColumn,
    CategoryColumn,
    NameColumn,
    BalanceColumn,
    LinksColumn,
    TagsColumn,			// The column holding the tags.
    MemoColumn,
    LastColumn
  };

  /// Returns the account that corresponds to the transactions (or
  /// NULL if there are no transactions).
  Account * account() const;


  /// Returns the QModelIndex corresponding to the given transaction
  QModelIndex index(Transaction * transaction);
};

/// A specific delegate to handle in particular the edition of
/// categories.
///
/// It should only handle the "category column", so you'd better set
/// it up using setItemDelegateForColumn.
class AccountItemDelegate : public QStyledItemDelegate {
  /// Target wallet
  Wallet * wallet;
public:
  AccountItemDelegate(Wallet * w);

  /// Reimplementations
  virtual QWidget * createEditor(QWidget * parent,
				 const QStyleOptionViewItem & option,
				 const QModelIndex & index) const;

  virtual void setEditorData(QWidget * editor,
			     const QModelIndex & index) const;


  virtual void setModelData(QWidget * editor, QAbstractItemModel * model,
			    const QModelIndex & index) const;
};

/// A weird delegate to be used with persistent editors for displaying
/// links...
class LinksItemDelegate : public QStyledItemDelegate {
public:

  /// Reimplementations
  virtual QWidget * createEditor(QWidget * parent,
				 const QStyleOptionViewItem & option,
				 const QModelIndex & index) const;

  virtual void setEditorData(QWidget * editor,
			     const QModelIndex & index) const;


  virtual void setModelData(QWidget * editor, QAbstractItemModel * model,
			    const QModelIndex & index) const;
};

#endif
