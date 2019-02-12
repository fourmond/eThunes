/*
    budgetdw.cc: Implementation of BudgetDW
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

#include <headers.hh>
#include <budgetdw.hh>

#include <htlabel.hh>
#include <wallet.hh>
#include <budget.hh>

#include <transaction.hh>

BudgetDW::BudgetDW(Wallet * w) : wallet(w)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);

  connect(*wallet, SIGNAL(changed(const Watchdog *)),
          SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

BudgetDW::~BudgetDW()
{
}

void BudgetDW::updateSummary()
{
  QString txt = "<h2>Budgets</h2>\n";


  txt += "<table>";

  QDate cd = QDate::currentDate();

  int nb = 0;

  int cols = 1;

  for(Budget & b : wallet->budgets) {
    if(b.exceptional)
      continue;                 // Don't mention it ;-)
    BudgetRealization * r = b.realizationForDate(cd);

    int planned = r->amountPlanned();
    int realized = r->amountRealized();
    if(planned == 0 && realized == 0)
      continue;                 // nothing to say !
    
    if(nb % cols == 0)
      txt += "<tr>";

    QString cur = realized >= planned ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(realized));


    txt += QString("<td>%1</td><td>%2/%3</td><td> -> %4</td>").
      arg(b.name).
      arg(cur).arg(Transaction::formatAmount(planned)).
      arg(r->period.endDate.toString("dd MMM"));

    nb += 1;
    if(nb % cols == 0)
      txt += "</tr>\n";
    else
      txt += "<td></td>";
  }

  txt += "</table>";

  summary->setText(txt);
}
