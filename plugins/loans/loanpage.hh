/**
    \file loanpage.hh
    The main page to interact with the Loan plugin
    Copyright 2011 by Vincent Fourmond

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

#ifndef __LOANPAGE_HH
#define __LOANPAGE_HH

#include <navigationpage.hh>
#include "loan.hh"

class HTLabel;

/// The main page of the Loan plugin.
class LoanPage : public NavigationPage {

  Q_OBJECT;

protected:

  static QHash<LoanPlugin *, LoanPage *> loanPages;

  LoanPlugin * plugin;

  /// The main summary (in the top ?)
  HTLabel * summary;

public:

  LoanPage(LoanPlugin * plugin);

  /// Returns the CollectionPage for the given collection, or create
  /// it if it doesn't exist yet.
  static LoanPage * getLoanPage(LoanPlugin * plugin);

  /// Returns the title of the page
  virtual QString pageTitle();


public slots:

  /// Update the page.
  void updatePage();

protected:

  /// Called to add a loan
  void addLoan();
};

#endif
