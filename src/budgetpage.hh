/**
    \file budgetpage.hh
    A page displaying informations about the budgets
    Copyright 2017 by Vincent Fourmond

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

#ifndef __BUDGETPAGE_HH
#define __BUDGETPAGE_HH


#include <navigationpage.hh>

class Wallet;
class HTLabel;
class HTDisplay;
class Budget;
class FlowingGridLayout;

/// This NavigationPage displays the informations about budgets
class BudgetPage : public NavigationPage {

  Q_OBJECT;

protected:

  /// A correspondance Wallet* -> BudgetPage
  static QHash<Wallet *, BudgetPage *> budgetPages;

  HTLabel * newBudgetSummary();

  Wallet * wallet;

  /// Layout for the individual budgets
  // FlowingGridLayout * topLayout;
  QGridLayout * topLayout;
  
  /// The HTLabels displaying the budgets
  QList<HTLabel *> budgetSummaries;

  QComboBox * summaryTableYear;
  

  /// The QLabel object displaying the rich text.
  HTLabel * top;

  /// The QLabel object displaying the rich text.
  HTDisplay * summary;

  

  void updateSummary();

public:

  BudgetPage(Wallet * a);

  /// Returns the title of the page
  virtual QString pageTitle() override;
  virtual ~BudgetPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static BudgetPage * getBudgetPage(Wallet * wallet);

  /// Returns the summary table for the given year
  QString summaryTableForYear(int year);

public slots:

  /// Creates a new budget with dummy name
  void addBudget();

  /// Prompts for a new name for the budget
  void promptNewName(Budget * budget);

  /// Prompts for a new amount
  void promptNewAmount(Budget * budget);

  /// Prompts for a new month
  void promptNewMonths(Budget * budget);

  /// Shows the summary for the given year
  void showYear(const QString & year);

  /// Toggles the "exceptional" flag
  void toggleExceptional(Budget * budget);

  /// Shows year-by-year statistics for the given budget
  void showStatistics(Budget * budget);

  /// Shows year-by-year statistics for all budgets
  void showStatistics();

};

#endif
