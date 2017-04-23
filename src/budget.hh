/**
    \file budget.hh
    Classes handling budgets
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


#ifndef __BUDGET_HH
#define __BUDGET_HH

#include <serializable.hh>
#include <linkable.hh>

class BudgetRealization;

/// This class represents a Budget.
/// Budgets have
/// @li a periodicity
/// @li an amount
/// @li and realizations.
///
/// The latter represent the list of transactions for a given period,
/// and the amount of money budgeted for that period (can change).
///
/// They are somehow similar to categories, but not exactly.
class Budget : public Serializable {
public:

  /// The provisional amount for the given period
  int amount;

  /// THe Budget's name
  QString name;

  /// The periodicity, in months
  int periodicity;

  /// The list of realizations
  WatchableList<BudgetRealization> realizations;

  virtual SerializationAccessor * serializationAccessor() override;

  virtual void finishedSerializationRead() override;


  
};

/// This class represents the realization of a budget, i.e. all the
/// transactions corresponding to the budget for a given period.
///
/// @a NB The user chooses which transaction goes into which
/// realization, she/he is not bound by the period.
///
/// The transactions are just stored as links, with the name
/// "budget-realization"
class BudgetRealization : public Linkable {
public:

  BudgetRealization();
    
  /// Beginning of the period
  QDate startDate;

  /// End of the period
  QDate endDate;

  /// Amount
  int amount;

  /// The budget this BudgetRealization belongs to.
  Budget * budget;

  virtual SerializationAccessor * serializationAccessor() override;

  virtual void followLink() override {
    /// @todo Everything
  };;

  virtual QString typeName() const override;
};


#endif
