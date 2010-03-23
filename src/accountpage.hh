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
#include <account-model.hh>
#include <navigationpage.hh>

/// This NavigationPage displays the contents of an account.
class AccountPage : public NavigationPage {

  Q_OBJECT;

  /// The account to be displayed.
  Account * account;

  /// The model that does it.
  AccountModel * model;

  /// And the widget to actually display stuff
  QTreeView * view;

  /// A label displaying summary information
  QLabel * accountSummary;

public:
  
  AccountPage(Account * a);
  
  /// Returns the title of the page
  virtual QString pageTitle();
  
  virtual ~AccountPage();

public slots:
  /// Updates the summary of the account
  void updateAccountSummary();
  
};

#endif
