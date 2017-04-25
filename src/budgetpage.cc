/*
    budgetpage.cc: Implementation of BudgetPage
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

#include <headers.hh>
#include <budgetpage.hh>
#include <statisticsmodel.hh>
#include <transactionlistdialog.hh>
#include <htlabel.hh>
#include <httarget-templates.hh>

BudgetPage::BudgetPage(Wallet * w) : wallet(w)
{

  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel;
  layout->addWidget(summary);
  updateSummary();
}


BudgetPage::~BudgetPage()
{
}

QString BudgetPage::pageTitle()
{
  return tr("Budgets");
}

void BudgetPage::updateSummary()
{
  QString text = tr("<h2>Budgets</h2>");
  text += HTTarget::linkToMember(tr("(new budget)"),
                                 this, &BudgetPage::addBudget)
    + "<p>\n";

  int totalPositive = 0;
  int totalNegative = 0;
  for(int i = 0; i < wallet->budgets.size(); i++) {
    Budget * budget = & wallet->budgets[i];
    text += tr("<h3>%1 %2</h3>").
      arg(budget->name).
      arg(HTTarget::linkToMember(tr("(change name)"),
                                 this, &BudgetPage::promptNewName, budget));
    text += tr("Amount: %1 %2<br>").
      arg(Transaction::formatAmount(budget->amount)).
      arg(HTTarget::linkToMember(tr("(change amount)"),
                                 this, &BudgetPage::promptNewAmount, budget));
    int ma = budget->amount / budget->periodicity;
    if(ma > 0)
      totalPositive += ma;
    else
      totalNegative += ma;
  }

  text += tr("<h3>Balance</h3><table>\n"
             "<tr><td>Income</td><td>%1</td></tr>\n"
             "<tr><td>Expense</td><td>%2</td></tr>\n"
             "<tr><td>Balance</td><td>%3</td></tr></table>\n").
    arg(Transaction::formatAmount(totalPositive)).
    arg(Transaction::formatAmount(totalNegative)).
    arg(Transaction::formatAmount(totalPositive + totalNegative));

  // Now, a summary of the realizations, just for the current month for now
  text += tr("<h3>Realizations</h3><table>\n");
  QDate cur = QDate::currentDate();
  for(int i = 0; i < wallet->budgets.size(); i++) {
    Budget * budget = & wallet->budgets[i];
    BudgetRealization * rel = budget->realizationForDate(cur, false);
    int amount = rel ? rel->amountRealized() : 0;
    text += QString("<tr><td>%1</td><td>%2/%3</td></tr>").
      arg(budget->name).arg(Transaction::formatAmount(amount)).
      arg(Transaction::formatAmount(budget->amount));
  }
  text += "</table>\n";
  
  
  summary->setText(text);
}

QHash<Wallet *, BudgetPage *> BudgetPage::budgetPages;

BudgetPage * BudgetPage::getBudgetPage(Wallet * wallet)
{
  if(! budgetPages.contains(wallet))
    budgetPages[wallet] = new BudgetPage(wallet);
  return budgetPages[wallet];
}

void BudgetPage::addBudget()
{
  wallet->budgets << Budget();
  Budget & bg = wallet->budgets.last();
  bg.name = "New budget";
  updateSummary();
}

void BudgetPage::promptNewName(Budget * budget)
{
  bool ok = false;
  QString newName =
    QInputDialog::getText(this, tr("New budget name"),
                          tr("Give a new name for the budget"),
                          QLineEdit::Normal, budget->name, &ok);
  if(ok) {
    budget->name = newName;
    updateSummary();
  }
}

void BudgetPage::promptNewAmount(Budget * budget)
{
  bool ok = false;
  int newAmount =
    QInputDialog::getInt(this, tr("New budget value"),
                          tr("Give a new value for the budget (in cents)"),
                         budget->amount, -2147483647, 2147483647,
                         1, &ok);
  if(ok) {
    budget->amount = newAmount;
    updateSummary();
  }
}
