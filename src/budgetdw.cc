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



  QDate cd = QDate::currentDate();

  int nb = 0;

  int cols = 1;

  class BudgetSummary {
  public:
    QString name;
    QDate end;
    int planned;
    int realized;
  };

  QHash<QDate, QList<BudgetSummary> > s;

  for(Budget & b : wallet->budgets) {
    if(b.exceptional)
      continue;                 // Don't mention it ;-)
    BudgetRealization * r = b.realizationForDate(cd);

    BudgetSummary sm;
    sm.end = r->period.endDate;
    sm.name = b.name;
    sm.planned = r->amountPlanned();
    sm.realized = r->amountRealized();
    if(sm.planned == 0 && sm.realized == 0)
      continue;                 // nothing to say !

    if(! s.contains(r->period.endDate))
      s[r->period.endDate] = QList<BudgetSummary>();

    s[r->period.endDate].append(sm);
  }

  QList<QDate> dates = s.keys();
  qSort(dates);

  for(const QDate & end : dates) {
    txt += QString("<h3>%1</h3>").
      arg(end.toString("dd MMM"));
    txt += "<table>";

    QList<BudgetSummary> ss = s[end];
    qSort(ss.begin(), ss.end(),
          [](const BudgetSummary & a, const BudgetSummary & b) -> bool {
        return a.name < b.name;
      });
    
    for(BudgetSummary & sm : ss) {
      QString cur = sm.realized >= sm.planned ?
        "<font color='green'>%1</font>" : 
        "<b><font color='red'>%1</font></b>";
      cur = cur.arg(Transaction::formatAmount(sm.realized));
      txt += QString("<tr><td>%1</td><td>%2</td><td>/</td><td>%3</td></tr>\n").
        arg(sm.name).
        arg(cur).arg(Transaction::formatAmount(sm.planned));
    }
    txt += "</table>";
  }

  summary->setText(txt);
}
