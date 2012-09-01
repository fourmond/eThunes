/**
    \file transactionlistdialog.hh
    A small generic dialog box to display list of pointers
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


#ifndef __TRANSACTIONLISTDIALOG_HH
#define __TRANSACTIONLISTDIALOG_HH

#include <transaction.hh>
#include <account.hh>
#include <category.hh>
#include <accountmodel.hh>
#include <transactionlistwidget.hh>

/// Dialog box to display transaction lists
///
/// \todo this class should handle resizing properly.
///
/// \todo Provide a way to display read-only transactions ! (this
/// should be done in AccountModel, probably, but here as well)
///
/// \todo Optionally display the account name ?
class TransactionListDialog : public QDialog {
protected:
  Q_OBJECT;

  /// the label at the top of the dialog box
  QLabel * topLabel;

  /// The model to display transactions
  AccountModel * model;

  /// The actual display of the transactions
  TransactionListWidget * view;

  /// The list of transactions to be displayed.
  TransactionPtrList list;

public:
  TransactionListDialog();

  ~TransactionListDialog();

public slots:

  /// Setup the dialog box to display the given list. The Wallet is
  /// necessary for the delegate for the Category column; it is taken
  /// from the first transaction if it is NULL (which means no
  /// delegate is created for Category edition)
  void displayList(const TransactionPtrList & list, const QString & label);

  /// Displays the checks of the given account.
  void displayChecks(Account * account);

  /// Displays the transactions of the given Category
  void displayCategory(Category * category, Wallet * wallet);

  /// Displays the transactions marked with the given Tag
  void displayTag(Tag * tag, Wallet * wallet);

  /// Displays the transactions for the given Month and the given
  /// account.
  void displayMonthlyTransactions(Account * account, int monthID, 
                                  int number = 1);

  /// Displays the transactions for the given Month and the given
  /// account.
  void displayMonthlyCategoryTransactions(Category * category,
                                          Account * account, 
                                          int monthID, 
                                          int number = 1);

public:

  /// Displays the checks of the given account.
  static void showChecks(Account * account);

  /// Displays the transactions of the given Category
  static void showCategory(Category * category, Wallet * wallet);

  /// Displays the transactions for the given Month and the given
  /// account.
  static void showMonthlyTransactions(Account * account, int monthID);

  /// same as above, but displays the given number of months
  static void showMonthlyTransactions(Account * account, int monthID, 
                                      int number = 1);

  /// Displays the transactions for the given Month and the given
  /// account.
  static void showMonthlyCategoryTransactions(Category * category,
                                              Account * account, 
                                              int monthID);

  static void showMonthlyCategoryTransactions(Category * category,
                                              Account * account, 
                                              int monthID, int number);


};

#endif
