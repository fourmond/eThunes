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
#include <widgetwrapperdialog.hh>
#include <flowinggridlayout.hh>
#include <evolvingitemwidget.hh>

#include <statistics.hh>

BudgetPage::BudgetPage(Wallet * w) : wallet(w)
{

  QVBoxLayout * layout = new QVBoxLayout(this);

  top = new HTLabel();
  layout->addWidget(top);
  
  topLayout = new QGridLayout();
  layout->addLayout(topLayout);

  QHBoxLayout * hz = new QHBoxLayout;

  hz->addWidget(new QLabel("<h2>Realizations for:</h2>"));

  summaryTableYear = new QComboBox;
  int firstYear = AtomicTransaction::dateFromID(wallet->firstMonthID()).year();
  int lastYear = QDate::currentDate().year() + 2;
  for(int y = firstYear; y <= lastYear; y++)
    summaryTableYear->addItem(QString::number(y));
  summaryTableYear->setEditable(false);

  connect(summaryTableYear,
          SIGNAL(activated(const QString &)),
          SLOT(showYear(const QString &)));

  hz->addWidget(summaryTableYear);
  hz->addStretch(1);

  layout->addLayout(hz);

  summary = new HTDisplay;
  layout->addWidget(summary, 1);

  updateSummary();
}


BudgetPage::~BudgetPage()
{
}

QString BudgetPage::pageTitle()
{
  return tr("Budgets");
}

HTLabel * BudgetPage::newBudgetSummary()
{
  HTLabel * rv = new HTLabel();
  int idx = budgetSummaries.size();
  int nbcols = 5;
  topLayout->addWidget(rv, idx / nbcols, idx % nbcols);
  budgetSummaries << rv;
  return rv;
}


/// A helper class to compute the
class BudgetBalance {
public:
  /// Planned budget
  int planned;

  /// Effectively realized budget
  int realized;

  /// The effective budgetprojected budget for the period
  int effective;

  Period period;

  BudgetBalance(int pl, int re, Period p) :
    planned(pl), realized(re), period(p)
  {
    QDate cd = QDate::currentDate();
    if(p.contains(cd)) {
      // status = Current;
      effective = planned > 0 ? std::max(realized, planned) :
        std::min(realized, planned);
    }
    else {
      if(p.startDate > cd) {
        // status = Future;
        effective = planned;
      }
      else {
        // status = Past;
        effective = realized;
      }
    }
  }

  void add(const BudgetBalance & o) {
    period = period.unite(o.period);
    effective += o.effective;
    realized += o.realized;
  };

  
};

QString BudgetPage::summaryTableForYear(int year)
{
  QDate cd = QDate::currentDate();
  int amounts[13];              // leaving out the "exceptional stuff"
  int budgeted[13];             // the purely budgeted thing
  int overallAmounts[13];       // not leaving them out
  QString text;

  QList<Budget *> budgets = wallet->budgets.pointerList();
  std::sort(budgets.begin(), budgets.end(), [](Budget * a, Budget * b) -> bool {
      return a->name < b->name;
    });

  for(int i = 0; i < sizeof(amounts)/sizeof(int); i++) {
    amounts[i] = 0;
    overallAmounts[i] = 0;
    budgeted[i] = 0;
  }

  // Now, a summary of the realizations, for the current year
  text += tr("<table><tr><td></td>\n");
  Period cy = Period::year(year);

  for(int i = 1; i <= 12; i++)
    text += tr("<th>%1 %2</th>").
      arg(QDate::shortMonthName(i)).arg(year);

  text += tr("<th>Total</th><th>(monthly)</th></tr>\n");
  
  for(int i = 0; i < budgets.size(); i++) {
    Budget * budget = budgets[i];
    QHash<Period, BudgetRealization *> rs = budget->realizationsForPeriod(cy);
    QList<Period> periods = rs.keys();
    // QTextStream o(stdout);
    qSort(periods);
    text += "<tr><td>" +
      HTTarget::linkToTransactionDisplay(budget->name,
                                         tr("Transactions for %1").arg(budget->name),
                                         [budget, cy]{
                                           TransactionPtrList trs = budget->allTransactions().transactionsForPeriod(cy);
                                           trs.sortByDate();
                                           return trs;
                                         })
      + "</td>";
    int tp = 0, te = 0;
    for(int i = 0; i < periods.size(); i++) {
      const Period & p = periods[i];
      BudgetRealization * r = rs[p];
      int planned, realized, effective, display;
      // o << "Period: " << p.startDate.toString()
      //   << " -> " << p.endDate.toString() << endl;
      if(r) {
        planned = r->amountPlanned();
        realized = r->amountRealized();
      }
      else {
        planned = budget->amount[p.startDate];
        realized = 0;
      }
      if(p.startDate > cd) {
        // planification
        effective = planned;
        display = effective;
      }
      else if(p.endDate < cd) {
        // past
        effective = realized;
        display = effective;
      }
      else {
        // current
        if(planned > 0) {
          effective = std::max(realized, planned);
        }
        else if(planned < 0) {
          effective = std::min(realized, planned);
        }
        else
          effective = realized;
        display = realized;
      }

      // counting it if not exceptional
      if(! budget->exceptional) {
        amounts[0] += effective;
        for(int i = p.startDate.month(); i <= p.endDate.month(); i++)
          amounts[i] += effective/p.months();
      }
      overallAmounts[0] += effective;
      for(int i = p.startDate.month(); i <= p.endDate.month(); i++)
        overallAmounts[i] += effective/p.months();
      
      budgeted[0] += planned;
      for(int i = p.startDate.month(); i <= p.endDate.month(); i++)
        budgeted[i] += planned/p.months();
      
      QString cur = display >= planned ?
        "<font color='green'>%1</font>" : 
        "<b><font color='red'>%1</font></b>";
      cur = cur.arg(Transaction::formatAmount(display));


      text += QString("<td colspan=%1 align='center' style='padding: 1px 3px;'>%2/%3</td>").
        arg(p.months()).
        arg(cur).arg(Transaction::formatAmount(planned));
      te += effective;
      tp += planned;
    }
    QString cur =  te >= tp ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(te));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1/%2</td>").
      arg(cur).arg(Transaction::formatAmount(tp));
    cur =  te >= tp ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(te/12));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1/%2</td>").
      arg(cur).arg(Transaction::formatAmount(tp/12));
    text += "</tr>\n";
  }

  text += tr("<tr><td>(unbudgeted)</td>");

  for(int i = 1; i <= 12; i++) {
    Period mo = Period::month(year, i);
    TransactionPtrList list = wallet->transactionsForPeriod(mo);
    list = BudgetRealization::realizationLessTransactions(list);
    TransactionListStatistics sts = list.statistics();
    amounts[i] += sts.totalAmount;
    amounts[0] += sts.totalAmount;
    overallAmounts[i] += sts.totalAmount;
    overallAmounts[0] += sts.totalAmount;
    QString cur =  sts.totalAmount >= 0 ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(sts.totalAmount));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
      arg(HTTarget::linkToTransactionDisplay(cur, tr("Unbudgeted transactions for ..."),
                                             [list]{ return list;}));
  }

  text += "<tr></tr>";

  text += tr("<tr><th>Budget</th>");
  for(int i = 0; i <= 12; i++) {
    int am = budgeted[(i + 1) % 13]; // so that last is amounts[0]
    QString cur =  am >= 0 ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(am));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
      arg(cur);
    if(i == 12) {
      cur =  am >= 0 ?
        "<font color='green'>%1</font>" : 
        "<b><font color='red'>%1</font></b>";
      cur = cur.arg(Transaction::formatAmount(am/12));
      text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
        arg(cur);
    }
  }

  text += tr("</tr><tr><th>Total</th>");
  for(int i = 0; i <= 12; i++) {
    int am = amounts[(i + 1) % 13]; // so that last is amounts[0]
    QString cur =  am >= 0 ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(am));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
      arg(cur);
    if(i == 12) {
      cur =  am >= 0 ?
        "<font color='green'>%1</font>" : 
        "<b><font color='red'>%1</font></b>";
      cur = cur.arg(Transaction::formatAmount(am/12));
      text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
        arg(cur);
    }
  }

  text += tr("</tr><tr><th>(all-inclusive) Total</th>");
  for(int i = 0; i <= 12; i++) {
    int am = overallAmounts[(i + 1) % 13]; // so that last is amounts[0]
    QString cur =  am >= 0 ?
      "<font color='green'>%1</font>" : 
      "<b><font color='red'>%1</font></b>";
    cur = cur.arg(Transaction::formatAmount(am));
    text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
      arg(cur);
    if(i == 12) {
      cur =  am >= 0 ?
        "<font color='green'>%1</font>" : 
        "<b><font color='red'>%1</font></b>";
      cur = cur.arg(Transaction::formatAmount(am/12));
      text += QString("<td align='center' style='padding: 1px 3px;'>%1</td>").
        arg(cur);
    }
  }
  text += "</tr>\n</table>\n";

  return text;
}

void BudgetPage::updateSummary()
{
  QString text = tr("<h2>Budgets</h2>");
  text +=
    HTTarget::linkToMember(tr("(update)"),
                           this, &BudgetPage::updateSummary) +
    HTTarget::linkToMember(tr("(new budget)"),
                           this, &BudgetPage::addBudget) +
    HTTarget::linkToMember(tr("(stats)"),
                           this, &BudgetPage::showStatistics)
    + "<p>\n";
  top->setText(text);

  int totalPositive = 0;
  int totalNegative = 0;
  QDate today = QDate::currentDate();

  QList<Budget *> budgets = wallet->budgets.pointerList();
  std::sort(budgets.begin(), budgets.end(), [](Budget * a, Budget * b) -> bool {
      return a->name < b->name;
    });
  
  for(int i = 0; i < budgets.size(); i++) {
    Budget * budget = budgets[i];
    text = tr("<h4>%1 %2 %4 (ex: %3)</h4>").
      arg(budget->name).
      arg(HTTarget::linkToMember(tr("(rename)"),
                                 this, &BudgetPage::promptNewName, budget)).
      arg(HTTarget::linkToMember(budget->exceptional ? "yes" : "no",
                                 this, &BudgetPage::toggleExceptional, budget)).
      arg(HTTarget::linkToMember("(S)",
                                 this, &BudgetPage::showStatistics, budget));
    ;

    text += tr("Amount: %1 %2<br>").
      arg(budget->amount.toString(&Transaction::formatAmount)).
      arg(HTTarget::linkToMember(tr("(change)"),
                                 this, &BudgetPage::promptNewAmount, budget));

    text += tr("Periodicity: %1 months %2").
      arg(budget->periodicity.months).
      arg(HTTarget::linkToMember(tr("(change)"),
                                 this, &BudgetPage::promptNewMonths, budget));


    
    int ma = budget->amount[today] / budget->periodicity.getMonths();
    if(ma > 0)
      totalPositive += ma;
    else
      totalNegative += ma;
    if(budgetSummaries.size() == i)
      newBudgetSummary();
    budgetSummaries[i]->setText(text);
  }
  for(int i = budgets.size(); i < budgetSummaries.size(); i++)
    budgetSummaries[i]->setText("");
  
  text = tr("<h3>Balance</h3><table>\n"
             "<tr><td>Income</td><td>%1</td></tr>\n"
             "<tr><td>Expense</td><td>%2</td></tr>\n"
             "<tr><td>Balance</td><td>%3</td></tr></table>\n").
    arg(Transaction::formatAmount(totalPositive)).
    arg(Transaction::formatAmount(totalNegative)).
    arg(Transaction::formatAmount(totalPositive + totalNegative));

  // QDate cd = QDate::currentDate();

  // int y = cd.year();

  // summary->setText(summaryTableForYear(y));
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

void BudgetPage::toggleExceptional(Budget * budget)
{
  budget->exceptional = ! budget->exceptional;
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
  EvolvingItem<int> newAmount = budget->amount;
  WidgetWrapperDialog dlg(new EvolvingIntWidget(&newAmount),
                          tr("New budget value for budget %1").arg(budget->name),
                          QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  dlg.setAttribute(Qt::WA_DeleteOnClose, false);

  if(dlg.exec() == QDialog::Accepted) {
    budget->amount = newAmount;
    updateSummary();
  }
}

void BudgetPage::promptNewMonths(Budget * budget)
{
  bool ok = false;
  int nm =
    QInputDialog::getInt(this, tr("New budget periodicity"),
                         tr("Periodicity for the budget in months"),
                         budget->periodicity.months,
                         1, 2147483647, 1, &ok);
  if(ok) {
    budget->periodicity.months = nm;
    updateSummary();
  }
}


void BudgetPage::showYear(const QString & year)
{
  int y = year.toInt();
  summary->setText(summaryTableForYear(y));
}

void BudgetPage::showStatistics()
{
  int ly = QDate::currentDate().year();
  int fy = ly;

  QList<Budget *> budgets = wallet->budgets.pointerList();
  std::sort(budgets.begin(), budgets.end(), [](Budget * a, Budget * b) -> bool {
      return a->name < b->name;
    });

  for(Budget * budget : budgets) {
    int y = budget->earliestDate().year();
    if(y < fy)
      fy = y;
  }
  // QTextStream o(stdout);
  // o << "First and last: " << fy << " -- " << ly << endl;

  QString s;
  s += QString("<h3>Statistics</h3>");
  s += "\n<table>";
  s += "<tr><th></th>";
  QHash<int, int> totals;
  QHash<int, int> ovTotals;     // including exceptional stuff
  for(int y = fy; y <= ly; y++) {
    s += QString("<th>%1</th>").arg(y);
    totals[y] = 0;
    ovTotals[y] = 0;
  }

  // o << "First and last: " << fy << " -- " << ly << endl;

  s+= "</tr>\n";

  for(Budget * budget : budgets) {
    s += QString("<tr><td><b>%1</b></td>").
      arg(budget->name);
    for(int y = fy; y <= ly; y++) {
      Period p = Period::year(y);
      int tot = 0;
      for(BudgetRealization * r : budget->realizationsForPeriod(p).values()) {
        if(r) {
          for(const AtomicTransaction * t : r->transactions())
            tot += t->getAmount();
        }
      }
      s += QString("<td align='right'>%1</td>").arg(Transaction::formatAmount(tot));
      if(! budget->exceptional)
        totals[y] += tot;
      ovTotals[y] += tot;
    }
    s += "</tr>\n";
  }

  s += QString("<tr><td><b>(unbudgeted)</b></td>");
  for(int y = fy; y <= ly; y++) {
    Period p = Period::year(y);
    int tot = 0;
    TransactionPtrList list = wallet->transactionsForPeriod(p);
    for(const AtomicTransaction * t : BudgetRealization::realizationLessTransactions(list))
      tot += t->getAmount();
    s += QString("<td align='right'>%1</td>").arg(Transaction::formatAmount(tot));
    totals[y] += tot;
    ovTotals[y] += tot;
  }
  
  s += "</tr>\n<tr></tr>\n";

  s += QString("<tr><td><b>Total</b></td>");
  for(int y = fy; y <= ly; y++)
    s += QString("<td align='right'><b>%1</b></td>").
      arg(Transaction::formatAmount(totals[y]));
  
  s += QString("</tr>\n<tr><td><b>All-inclusive</b></td>");
  for(int y = fy; y <= ly; y++)
    s += QString("<td align='right'><b>%1</b></td>").
      arg(Transaction::formatAmount(ovTotals[y]));
  s += "</tr>\n</table>\n";

  // o << s << endl;

  (new WidgetWrapperDialog(new QLabel(s)))->exec();
}

void BudgetPage::showStatistics(Budget * budget)
{
  int fy = budget->earliestDate().year();
  int ly = QDate::currentDate().year();
  QTextStream o(stdout);
  o << "Found years: " << fy << endl;

  QHash<int, CategorizedStatistics> stats;
  while(fy <= ly) {
    Period p = Period::year(fy);
    for(BudgetRealization * r : budget->realizationsForPeriod(p).values()) {
      if(r) {
        for(const AtomicTransaction * t : r->transactions())
          stats[fy].addTransaction(t, false);
      }
    }
    ++fy;
  }

  QString s;
  s += QString("<h3>%1</h3>").arg(budget->name);
  s += "\n<table>";

  QList<int> years = stats.keys();
  QHash<int,QStringList> rows;
  std::sort(years.begin(), years.end());
  s += "<tr>";
  int nb = 0;
  for(int y : years) {
    s += QString("<th>%1</th><th></th>").arg(y);
    int t = 0;
    for(auto i : stats[y].categorize()) {
      rows[y] << QString("<td>%1</td><td>%2</td>").
        arg(i.category).
        arg(Transaction::formatAmount(i.amount));
      t += i.amount;
    }
    rows[y].insert(0, QString("<td><b>Total</b></td><td>%1</td>").
                   arg(Transaction::formatAmount(t)));
    if(nb < rows[y].size())
      nb = rows[y].size();
  }
  s += "</tr>";

  for(int i = 0; i < nb; i++) {
    s += "<tr>";
    for(int y : years)
      s += rows[y].value(i, "<td></td><td></td>");
    s += "</tr>\n";
  }

  s += "\n</table>";

  (new WidgetWrapperDialog(new QLabel(s)))->exec();
}
