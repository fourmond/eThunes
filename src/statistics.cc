/*
    statistics.cc: implementation of the Statistics class
    Copyright 2011 by Vincent Fourmond

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
#include <statistics.hh>

#include <wallet.hh>
#include <account.hh>

#include <transactionlistdialog.hh>

#include <httarget-templates.hh>

void CategorizedStatistics::addTransaction(const AtomicTransaction * t, 
                                           bool topLevel)
{
  // We ignored transactions flagged as internal move
  if(t->hasNamedLinks("internal move"))
    return;
  const Category * cat = t->getCategory();
  if(cat && topLevel)
    cat = cat->topLevelCategory();
  (*this)[cat].addTransaction(t);
}

QList<CategorizedStatistics::Item> CategorizedStatistics::categorize() const
{
  QList<Item> retval;
  for(const_iterator i = begin(); i != end(); i++)
    retval.append(Item(i.key(), i.value().totalAmount));
  qSort(retval);
  return retval;
}

//////////////////////////////////////////////////////////////////////


PeriodicCategorizedStatistics::PeriodicCategorizedStatistics(const QList<Account *>& ac, const Periodic & periodic) :
  accounts(ac), periodicity(periodic)
{
  wallet = (accounts.size() > 0 && accounts.first() ? accounts.first()->wallet : NULL);
}

/// Adds a Transaction to the statistics.
void PeriodicCategorizedStatistics::addTransaction(const AtomicTransaction * t, 
                                                   bool topLevel)
{
  Period p = periodicity.periodForDate(t->getDate());
  (*this)[p].addTransaction(t, topLevel);
}



/// @todo Using TransactionList for that really isn't the right thing
/// to do !
QHash<QString, TransactionList> PeriodicCategorizedStatistics::listsForDisplay() const
{
  QHash<QString, TransactionList> ret;
  // QList<int> ks = keys();
  // qSort(ks);

  // for(int i = 0; i < ks.size(); i++) {
  //   QList<CategorizedStatistics::Item> lst = value(ks[i]).categorize();
  //   for(int j = 0; j < lst.size(); j++) {
  //     ret[lst[j].category].
  //       append(Transaction(dateOfBucket(ks[i]), lst[j].amount));
  //   }
  // }
  return ret;
}

QString PeriodicCategorizedStatistics::elementName(const Period & period) const
{
  QList<Account * > acs = accounts;
  return HTTarget::
    linkToFunction(periodicity.periodName(period),
                   [acs, period] () {
                     TransactionListDialog::showTransactionsForPeriod(acs, period);
                   });
}

QString PeriodicCategorizedStatistics::categoryName(const Period & period,
                                                    const QString & name,
                                                    const QString & disp) const
{
  QList<Account * > acs = accounts;
  Category * cat = wallet->namedCategory(name);
  return HTTarget::
    linkToFunction(disp.isEmpty() ? name : disp,
                   [acs, period, cat] () {
                     TransactionListDialog::showCategoryTransactionsForPeriod(cat, acs, period);
                   }
                   );
}

//////////////////////////////////////////////////////////////////////

Statistics::Statistics(const TransactionPtrList & lst, bool topLevel)
{
  QSet<Account *> acs;
  for(int i = 0; i < lst.size(); i++) {
    Account * ac = lst[i]->getAccount();
    if(ac)
      acs << ac;
  }
        
  stats["monthly"] =
    new PeriodicCategorizedStatistics(acs.toList(), Periodic::monthly); 
  stats["trimester"] =
    new PeriodicCategorizedStatistics(acs.toList(), Periodic::trimester); 
  stats["yearly"] =
    new PeriodicCategorizedStatistics(acs.toList(), Periodic::yearly); 
  stats["schoolyear"] =
    new PeriodicCategorizedStatistics(acs.toList(), Periodic::schoolYear); 

  for(int i = 0; i < lst.size(); i++) {
    for(auto j = stats.begin() ; j != stats.end(); j++) {
      (*j)->addTransaction(lst[i], topLevel);
    }
  }
}

Statistics::~Statistics()
{
  for(auto i = stats.begin(); i != stats.end(); i++)
    delete *i;
}


/// @todo This is a good idea, but maybe it should be up to the user
/// to choose whether we're displaying the average or the sum.
QString Statistics::htmlStatistics(PeriodicCategorizedStatistics * which,
                                   int number, int maxDisplay, 
                                   bool monthlyAverage) const
{
  QList<QStringList> columns;
  QList<Period> periods = which->keys();
  qSort(periods);
  // int div = (monthlyAverage ? which->monthNumber() : 1);
  int div = 1;
  for(auto i = periods.rbegin(); i != periods.rend(); i++) {
    QStringList c1, c2;
    c1 << QString("<b>%1</b>").arg(which->elementName(*i));
    c2 << "";
    QList<CategorizedStatistics::Item> items = 
      (*which)[*i].categorize();


    c1 << which->categoryName(*i, items.last().category, "Revenues");
    c2 << Transaction::formatAmount(items.last().amount/div);

    int rest = 0;
    for(int j = 0; j < items.size()-1; j++)
      rest += items[j].amount;
    int balance = items.last().amount + rest;

    c1 << ( balance < 0 ? "Deficit<hr/>" : "Excedent<hr/>");
    c2 << QString( balance < 0 ? 
                   "<b><font color='red'>%1</font></b><hr/>" : 
                   "<b><font color='green'>%1</font></b><hr/>").
      arg(Transaction::formatAmount((items.last().amount + rest)/div));

    c1 << "Expenses";
    c2 << Transaction::formatAmount(rest/div);

    for(int j = 0; j < std::min(items.size(), maxDisplay); j++) {
      c1 << which->categoryName(*i, items[j].category);
      c2 << Transaction::formatAmount(items[j].amount/div);
    }
    columns << c1 << c2;
  }
  QString ret = "<table>\n";
  // Won't work when there are very little
  for(int i = 0; i < maxDisplay+3; i++) {
    ret += "<tr>";
    int skip = 2;
    for(int j = 0; j < columns.size(); j++) {
      if(columns[j].size() > i)
        ret += QString("<td %2 style='padding: 1px 3px;'>%1</td>").
          arg(columns[j][i]).arg( ((j % skip) == 0 ? "" : "align='right'"));
      else 
        ret += "<td style='padding: 1px 3px;'></td>";
    }
    ret += "</tr>\n";
  }
  ret += "</table>";
  return ret;
}




/// @todo This just doesn't substitute for a proper widget. Here are
/// few ideas for it:
/// @li it should provide graphics display (possibly by alternance)
/// where each data point would be very visible, with a neat tooltip
/// and a context menu for showing transactions ?
QString Statistics::htmlStatistics(const QString & which, int months, 
                                   int maxDisplay, bool monthlyAverage) const
{
  return htmlStatistics(stats[which], months, maxDisplay, monthlyAverage);
}

