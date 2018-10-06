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
#include <periodic.hh>
#include <evolvingitem.hh>

class BudgetRealization;
class AtomicTransaction;
class TransactionPtrList;

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
  EvolvingItem<int> amount;

  /// THe Budget's name
  QString name;

  /// The periodicity, a Periodic object
  Periodic periodicity;

  /// Whether the budget corresponds to something exceptionnal or not
  bool exceptional;

  Budget();

  /// The list of realizations
  WatchableList<BudgetRealization> realizations;

  virtual SerializationAccessor * serializationAccessor() override;

  virtual void finishedSerializationRead() override;

  /// Adds the given transaction to the BudgetRealization within the
  /// given period.
  void addTransaction(const AtomicTransaction * trans,
                      const QDate & date = QDate());

  /// Returns the realization for the date, creating one if necessary
  BudgetRealization * realizationForDate(const QDate & date, bool create = true);

  /// Returns all the realizations for the given period, indexed by
  /// their period. Creates them if @a create is set to true.
  QHash<Period, BudgetRealization *> realizationsForPeriod(const Period & period,
                                                           bool create = false);

  /// Returns all the transactions for all the budget's realizations.
  TransactionPtrList allTransactions() const;
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

  /// Period
  Period period;


  /// The budget this BudgetRealization belongs to.
  Budget * budget;

  virtual SerializationAccessor * serializationAccessor() override;

  virtual void followLink() override;

  virtual QString typeName() const override;

  virtual QString publicLinkName() const override;

  bool contains(const QDate & date) const;

  /// Just adds the given transaction to the appropriately-named links
  void addTransaction(AtomicTransaction * transaction);

  /// Returns the planned amount corresponding to the period.
  int amountPlanned();

  /// Returns the amount realized in the transactions connected to
  /// this realization.
  int amountRealized();

  /// Returns the transactions.
  TransactionPtrList transactions() const;

  static TransactionPtrList realizationLessTransactions(const TransactionPtrList & lst);

};



#endif
