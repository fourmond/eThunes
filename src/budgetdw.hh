/**
    \file budgetdw.hh
    DashboarWidget for representing the budgets
    Copyright 2019 by Vincent Fourmond

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

#ifndef __BUDGETDW_HH
#define __BUDGETDW_HH

#include <dashboardwidget.hh>


class Wallet;
class HTLabel;

/// A dashboard widget representing the budgets
class BudgetDW : public DashboardWidget {

  Q_OBJECT;

  /// The wallet whose contents we should display.
  Wallet * wallet;

  /// Essentially the whole display ?
  HTLabel * summary;

public:
  BudgetDW(Wallet * w);
  virtual ~BudgetDW();

public slots:
  void updateSummary();

};

#endif
