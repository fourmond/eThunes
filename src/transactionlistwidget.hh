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

#include <accountmodel.hh>

/// This Widget is a thin wrapper around a QTreeView with an
/// AccoutModel as model.
///
/// \todo Activate multiple selection and provide popup menus to
/// choose categories.
/// 
/// \todo Optionally remove the balance ?
/// 
class TransactionListWidget : public QWidget {

  Q_OBJECT;
protected:
  /// \todo Find a way to disable columns on demand.
  void setupFrame();

  void setupTreeView();

public:
  
  TransactionListWidget(QWidget * parent = 0);
  TransactionListWidget(TransactionList *transactions, 
			QWidget * parent = 0);
  TransactionListWidget(TransactionPtrList *transactions, 
			QWidget * parent = 0);

  void showTransactions(TransactionList *transactions);
  void showTransactions(TransactionPtrList *transactions);




  /// And the widget to actually display stuff
  QTreeView * view;

  /// The model that does it.
  AccountModel * model;
  
  ~TransactionListWidget();

signals:


public slots:

  /// Shows the given transaction
  void showTransaction(Transaction * transaction);
  
};

#endif