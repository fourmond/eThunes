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


#ifndef __ACCOUNT_MODEL_HH
#define __ACCOUNT_MODEL_HH

#include <transaction.hh>

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
class AccountModel : public QAbstractItemModel {

  Q_OBJECT;

  /// List of transactions:
  ///
  /// \todo This is no accessor, and doesn't care for
  /// modifications. The final type should be a QObject child with
  /// appropriate signaling mechanism.
  TransactionList * transactions;

protected:
  /// Returns the transaction corresponding to the index, or NULL if
  /// invalid or root.
  Transaction * indexedTransaction(QModelIndex index) const;
  
public:
  /// The transactions to be displayed.
  AccountModel(TransactionList * transactions);

  enum {
    DateColumn,
    AmountColumn,
    BalanceColumn,
    CategoryColumn,
    NameColumn,
    MemoColumn,
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

  virtual Qt::ItemFlags flags(const QModelIndex & index) const;

  virtual bool setData(const QModelIndex & index, const QVariant & value, 
		       int role = Qt::EditRole);  

public slots:
  
  /// Radically changes things that have changed ;-)...
  void accountChanged();
};

#endif
