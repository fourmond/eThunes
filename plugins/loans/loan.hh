/**
   \file loan.hh
   Plugin to keep track of loans
   Copyright 2012 by Vincent Fourmond

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


#ifndef __LOAN_HH
#define __LOAN_HH

#include <plugin.hh>
#include <linkable.hh>
#include <transactionlists.hh>


class LoanPlugin;

/// This represents a single Loan
///
/// All transactions that pay back this loan are linked to it using
/// the Linkable framework
class Loan : public Linkable {
protected:

  /// Computes all debt-related things, based on the transactions
  /// linked to it
  void computeDebt();

  /// The amount we still owe
  int amountLeft;

  /// The total that was paid so far
  int totalPaid;

  /// The total amount of interests that was paid
  int paidInterests;

  /// The number of months the loan has been running so far
  int monthsRunning;

  /// Monthly rate (real units, ie 1 + percent)
  double monthlyRate;

  /// Planned monthly payment (negative means keep going on with the
  /// last)
  int plannedMonthlyPayment;

  /// Last monthly payment
  int lastMonthlyPayment;

  int effectiveMonthlyPayment;

  /// Planned stuff:
  int remainingMonths;
  int leftToPay;

  /// Matching transactions...
  TransactionPtrList matchingTransactions;

public:

  /// A public name
  QString name;

  /// The date the Loan was contracted.
  QDate dateContracted;

  /// The amount of the Loan
  int amount;

  /// The yearly interest rate.
  double yearlyRate;

  /// A distinctive sign to find matching transactions, such as
  /// something in the memo
  ///
  /// @todo This may be implemented rather on the filter side, ie
  /// automatically link to a specific target ?
  QString matcher;

  virtual SerializationAccessor * serializationAccessor();


  virtual QString typeName() const;

  virtual QString publicTypeName() const;

  virtual void followLink();

  LoanPlugin * targetPlugin;

  Loan();

  /// Renders as HTML
  QString html();

protected:

  void promptForName();
  void promptForAmount();
  void promptForDate();
  void promptForRate();
  void promptForMatcher();
  void promptForMonthlyPayment();

  void findMatchingTransactions();


  /// Makes sure the page is updated...
  ///
  /// @todo This should diseappear in profit of the use of Watchable
  /// stuff.
  void updatePage();
  
};


/// This plugin helps to keep track of the loans one have.
///
/// @todo Maybe there should be a way for a plugin to add an "item" in
/// the "balance" panel of the Cabinet ?
class LoanPlugin : public Plugin {
public:
  virtual QString typeName() const {
    return "loan";
  };

  /// A list of loans...
  QList<Loan> loans;

  virtual NavigationPage * pageForPlugin();

  virtual SerializationAccessor * serializationAccessor();

  virtual void finishedSerializationRead();

  LoanPlugin();

};


#endif
