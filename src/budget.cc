/*
    budget.cc: Budget and related class
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
#include <budget.hh>

#include <transaction.hh>
#include <transactionlists.hh>
#include <evolvingitemwidget.hh>

#include <navigationwidget.hh>
#include <budgetpage.hh>
#include <account.hh>

Budget::Budget() : amount(0), periodicity(1), exceptional(false)
{
  
}

SerializationAccessor * Budget::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name);
  ac->addAttribute("amount", &amount);
  ac->addScalarAttribute("periodicity", &periodicity);
  ac->addScalarAttribute("exceptional", &exceptional);
  ac->addListAttribute("realization", &realizations);
  return ac;
}

void Budget::finishedSerializationRead()
{
  for(int i = 0; i < realizations.size(); i++)
    realizations[i].budget = this;
}

BudgetRealization * Budget::realizationForDate(const QDate & date, bool create)
{
  if(! date.isValid())
    return NULL;
  for(int i = 0; i < realizations.size(); i++) {
    if(realizations[i].contains(date))
      return &realizations[i];
  }
  if(! create)
    return NULL;
  realizations << BudgetRealization();
  BudgetRealization & lst = realizations.last();
  lst.budget = this;
  lst.period = periodicity.periodForDate(date);
  return &lst;
}

QHash<Period, BudgetRealization *> Budget::realizationsForPeriod(const Period & period,
                                                                 bool create)
{
  QHash<Period, BudgetRealization *> rv;
  Period cur = periodicity.periodForDate(period.startDate);
  do {
    rv[cur] = realizationForDate(cur.startDate, create);
    cur = periodicity.nextPeriod(cur);
  } while(cur.startDate <= period.endDate);
  return rv;
}

TransactionPtrList Budget::allTransactions() const
{
  TransactionPtrList lst;
  for(int i = 0; i < realizations.size(); i++)
    lst.append(realizations[i].transactions());
  return lst;
}


QDate Budget::earliestDate() const
{
  QDate er;
  for(const BudgetRealization & r : realizations) {
    if((!er.isValid()) || (er > r.period.startDate))
      er = r.period.startDate;
  }
  return er;
}





//////////////////////////////////////////////////////////////////////

BudgetRealization::BudgetRealization() : budget(NULL)
{
}

SerializationAccessor * BudgetRealization::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("period", &period);
  addLinkAttributes(ac);
  return ac;
}

QString BudgetRealization::typeName() const
{
  return "budget-realization";
}

QString BudgetRealization::publicLinkName() const
{
  QString rv = QObject::tr("B: %1 %2").
    arg(budget->name).
    arg(budget->periodicity.periodName(period));
  return rv;
}

bool BudgetRealization::contains(const QDate & date) const
{
  return period.contains(date);
}

void BudgetRealization::addTransaction(AtomicTransaction * transaction)
{
  // We first remove any link that exists previously
  QList<BudgetRealization*> previousBudgets =
    transaction->links.typedLinks<BudgetRealization>("budget-realization");
  for(BudgetRealization * tg : previousBudgets)
    transaction->removeLink(tg, "budget-realization");
  addLink(transaction, "budget-realization");
}

void BudgetRealization::followLink()
{
  QList<AtomicTransaction*> transactions =
    links.typedLinks<AtomicTransaction>("budget-realization");
  if(transactions.size() > 0) {
    Wallet * w = transactions[0]->getAccount()->wallet;
    NavigationWidget::gotoPage(BudgetPage::getBudgetPage(w));
  }
}

TransactionPtrList BudgetRealization::transactions() const
{
  QList<AtomicTransaction*> transactions =
    links.typedLinks<AtomicTransaction>("budget-realization");
  TransactionPtrList lst;
  lst.append(transactions);
  return lst;
}

int BudgetRealization::amountPlanned() const
{
  return budget->amount[period.startDate];
}


int BudgetRealization::amountRealized() const
{
  int rv = 0;
  QList<AtomicTransaction*> transactions =
    links.typedLinks<AtomicTransaction>("budget-realization");

  for(int i = 0; i < transactions.size(); i++)
    rv += transactions[i]->getAmount();
    
  return rv;
}

TransactionPtrList BudgetRealization::realizationLessTransactions(const TransactionPtrList & lst)
{
  TransactionPtrList rv;
  for(int i = 0; i < lst.size(); i++) {
    AtomicTransaction * t = lst[i];
    QList<BudgetRealization*> rl =
      t->links.typedLinks<BudgetRealization>("budget-realization");
    if(rl.size() == 0)
      rv << t;
  }
  return rv;    
}
  
//////////////////////////////////////////////////////////////////////

void BudgetStats::addRealization(const BudgetRealization * realization)
{
  PeriodStats s;
  s.period = realization->period;
  s.planned = realization->amountPlanned();
  s.realized = realization->amountRealized();
  stats << s;
}

void BudgetStats::addBudget(const Budget * budget, const Period & period)
{
  PeriodStats s;
  s.period = period;
  s.planned = budget->amount[period.startDate];
  s.realized = 0;
  stats << s;
}

int BudgetStats::realized() const
{
  int rv = 0;
  for(const PeriodStats & s : stats)
    rv += s.realized;
  return rv;
}

int BudgetStats::planned() const
{
  int rv = 0;
  for(const PeriodStats & s : stats)
    rv += s.planned;
  return rv;
}

int BudgetStats::effective(const QDate & ref) const
{
  int effective = 0;
  for(const PeriodStats & s : stats) {
    if(s.period.startDate > ref) {
      // planification
      effective += s.planned;
    }
    else if(s.period.endDate < ref) {
      // past
      effective += s.realized;
    }
    else {
      // current
      if(s.planned > 0) {
        effective += std::max(s.realized, s.planned);
      }
      else if(s.planned < 0) {
        effective += std::min(s.realized, s.planned);
      }
      else
        effective += s.realized;
    }
  }
  return effective;
}

