/**
    \file accountpage.hh
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

#ifndef __ACCOUNTPAGE_HH
#define __ACCOUNTPAGE_HH

#include <account.hh>
#include <accountmodel.hh>
#include <navigationpage.hh>
#include <transactionlistwidget.hh>

class HTLabel;

/// This NavigationPage displays the contents of an account.
///
/// \todo It feels like I need some text searchbox here. Incremental
/// search with Ctrl+F (and going on),
/// (emacs|iceweasel)-like. Temporary storage of the transactions
/// found would help speedup incremental search.
class AccountPage : public NavigationPage {

  Q_OBJECT;

  /// The account to be displayed.
  Account * account;

  /// And the widget to actually display stuff
  TransactionListWidget * view;

  /// A label displaying summary information
  HTLabel * accountSummary;

  /// A correspondance Account* -> AccountPage
  static QHash<Account *, AccountPage *> accountPages;

public:

  AccountPage(Account * a);

  /// Returns the title of the page
  virtual QString pageTitle();

  virtual ~AccountPage();

public slots:
  /// Updates the summary of the account
  void updateAccountSummary();

  /// Prompts for renaming the account
  void renameAccount();

  /// Selects and shows the given Transaction, if it is visible
  void showTransaction(Transaction * transaction);

  /// Displays the balance
  void displayBalance();

  /// Adds a previsional transaction (dated today)
  void addPrevisionalTransaction();

public:

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static AccountPage * getAccountPage(Account * account);


};

#endif
