/*
    transaction-list-model.cc: model for transaction lists
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

#include <headers.hh>
#include <account.hh>
#include <account-model.hh>

AccountModel::AccountModel(QList<Transaction> t) :
  transactions(t)
{
}

const Transaction * AccountModel::indexedTransaction(QModelIndex index) const
{
  if(index.isValid() && index.internalId() >= 0 && 
     index.internalId() < transactions.count() )
    return &transactions[index.internalId()];
  return NULL;
}

QModelIndex AccountModel::index(int row, int column, 
					const QModelIndex & parent) const
{
  // printf("Index: %d %d %d\n", row, column, parent.internalId());
  if(parent.isValid()) 
    return createIndex(row, column, row);
  else
    return createIndex(row, column, -1);
}

QModelIndex AccountModel::parent(const QModelIndex & index) const
{
  // printf("Parent: %d %d %d\n", index.row(), index.column(), index.internalId());
  if(index.isValid()) {
    if(index.internalId() >= 0) 
      return createIndex(0, 0, -1);
    else
      return QModelIndex();
  }
  else
    return QModelIndex();
}

int AccountModel::rowCount(const QModelIndex & index) const
{
  // printf("row: %d %d %d\n", index.row(), index.column(), index.internalId());
  if(index.isValid()) {
    if(index.internalId() >= 0) 
      return 0;
    else 
      return transactions.count();
  }
  return 0;
}

int AccountModel::columnCount(const QModelIndex & /*index*/) const
{  
  return 5;
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, 
				  int role) const 
{
  if(role == Qt::DisplayRole) {
    switch(section) {
    case 0: return QVariant(tr("Date"));
    case 1: return QVariant(tr("Amount"));
    case 2: return QVariant(tr("Balance"));
    case 3: return QVariant(tr("Name"));
    case 4: return QVariant(tr("Memo"));
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}


QVariant AccountModel::data(const QModelIndex& index, int role) const
{
  const Transaction *t = indexedTransaction(index);
  if(! t)
    return QVariant();
  if(role == Qt::DisplayRole) {
    switch(index.column()) {
    case 0: return QVariant(t->date);
    case 1: return QVariant(t->amount * 0.01);
    case 2: return QVariant(t->balance * 0.01);
    case 3: return QVariant(t->name);
    case 4: return QVariant(t->memo);
    default:
      return QVariant();
    }
  }
  else
    return QVariant();
}
