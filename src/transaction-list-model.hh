/** 
    \file transaction-list-model.hh
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


#ifndef __TRANSACTION_LIST_MODEL_HH
#define __TRANSACTION_LIST_MODEL_HH

/// Class in charge of organising the data of a list of Transaction
/// objects into something viewable with a Tree or Table view.
class TransactionListModel : public QAbstractItemModel {
  /// List of transactions:
  ///
  /// \todo This is no accessor, and doesn't care for
  /// modifications. The final type should be a QObject child with
  /// appropriate signaling mechanism.
  ///
  /// \todo We simply should use an Account instead of this.
  QList<Transaction> transactions;

protected:
  /// Returns the transaction corresponding to the index, or NULL if
  /// invalid or root.
  const Transaction * indexedTransaction(QModelIndex index) const;
  
public:
  /// The transactions to be displayed.
  TransactionListModel(QList<Transaction> transactions);


  virtual QModelIndex index(int row, int column, 
			    const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex&) const;

  virtual int rowCount(const QModelIndex&) const;

  virtual int columnCount(const QModelIndex&) const;

  virtual QVariant data(const QModelIndex&, int) const;
};

#endif
