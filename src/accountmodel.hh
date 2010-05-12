/** 
    \file account-model.hh
    An item model providing transaction data
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


#ifndef __ACCOUNTMODEL_HH
#define __ACCOUNTMODEL_HH

#include <transaction.hh>
#include <transactionlists.hh>

/// Class in charge of organising the data of a list of Transaction
/// objects into something viewable with a Tree or Table view.
///
/// \todo Ideas:
/// \li color background (or whathever) by month (possibly moving)
/// \li color balance/amount by credit/debit
/// \li color lines/specific parts according to the category
/// More ideas ?
///
/// \todo This class should be more general and handle list of
/// Transaction * instead of the current TransactionList; this would
/// make it fairly trivial to view/modify transactions based on other
/// criteria (Category, for intance) rather than the current model
/// here. I'd better modify this before it is too painful to do.
///
/// \todo Tanya says it would be much nicer if the transactions were
/// grouped by month in a tree-like fashion. This is easy, but maybe a
/// little long ?
class AccountModel : public QAbstractItemModel {

  Q_OBJECT;

  /// List of transactions:
  TransactionList * transactions;
  
  /// Or this way:
  TransactionPtrList * transactionsPtr;
  

protected:
  /// Returns the transaction corresponding to the index, or NULL if
  /// invalid or root.
  Transaction * indexedTransaction(QModelIndex index) const;

  /// returns transaction at the given index.
  Transaction * indexedTransaction(int idx) const;

  /// Static cache for icons
  static QHash<QString, QIcon> statusIcons;

  /// Returns a small icon (16x16) representing a given status
  static const QIcon & statusIcon(const QString & status);
  
public:
  AccountModel(TransactionList * transactions);
  AccountModel(TransactionPtrList * transactions);

  enum {
    RecentColumn,		/// Whether the transaction is new or not ?
    DateColumn,
    AmountColumn,
    CategoryColumn,
    NameColumn,
    BalanceColumn,
    MemoColumn,
    LastColumn
  };


  /// Returns the QModelIndex corresponding to the given transaction
  QModelIndex index(Transaction * transaction);

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

public slots:
  
  /// Radically changes things that have changed ;-)...
  void accountChanged();

protected:
  
  /// Number of transactions in the list
  int transactionCount() const {
    if(transactions) 
      return transactions->count();
    if(transactionsPtr)
      return transactionsPtr->count();
    return 0;
  };
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

#endif
