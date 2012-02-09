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

/// @todo ignore transactions that have an "internal move" link.
void CategorizedStatistics::addTransaction(const Transaction * t, 
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


PeriodicCategorizedStatistics::PeriodicCategorizedStatistics(Account * ac) {
  account = ac;
  wallet = (account ? account->wallet : NULL);
}

//////////////////////////////////////////////////////////////////////

void CategorizedMonthlyStatistics::addTransaction(const Transaction * t, 
                                                  bool tl)
{
  (*this)[t->monthID()].addTransaction(t, tl);
}


QString CategorizedMonthlyStatistics::elementName(int id) const
{
  return HTTarget::
    linkToFunction(Utils::monthName(Transaction::dateFromID(id), false),
                   &TransactionListDialog::showMonthlyTransactions,
                   account, id);
}

QString CategorizedMonthlyStatistics::categoryName(int id,
                                                   const QString & name,
                                                   const QString & disp) const
{
  return HTTarget::
    linkToFunction(disp.isEmpty() ? name : disp,
                   &TransactionListDialog::showMonthlyCategoryTransactions,
                   wallet->namedCategory(name),
                   account, id);
}

//////////////////////////////////////////////////////////////////////

void CategorizedTrimesterStatistics::addTransaction(const Transaction * t, 
                                                    bool tl)
{
  /// @bug This assumes that the monthID % 3 is 0 for the beginning of
  /// the year.
  (*this)[t->monthID() - (t->monthID() % 3)].addTransaction(t, tl);
}

QString CategorizedTrimesterStatistics::elementName(int id) const
{

  return HTTarget::
    linkToFunction(Utils::monthName(Transaction::dateFromID(id), false) + 
                   " - " +
                   Utils::monthName(Transaction::dateFromID(id + 2), false),
                   &TransactionListDialog::showMonthlyTransactions,
                   account, id, 3);
}

QString CategorizedTrimesterStatistics::categoryName(int id,
                                                     const QString & name,
                                                     const QString & disp) const
{
  return HTTarget::
    linkToFunction(disp.isEmpty() ? name : disp,
                   &TransactionListDialog::showMonthlyCategoryTransactions,
                   wallet->namedCategory(name),
                   account, id, 3);
}

//////////////////////////////////////////////////////////////////////

void CategorizedYearlyStatistics::addTransaction(const Transaction * t, 
                                                    bool tl)
{
  (*this)[t->getDate().year()].addTransaction(t, tl);
}

QString CategorizedYearlyStatistics::elementName(int id) const
{
  /// @bug This assumes that the monthID = 12 * year
  return HTTarget::
    linkToFunction(QString::number(id),
                   &TransactionListDialog::showMonthlyTransactions,
                   account, id*12, 12);
}

QString CategorizedYearlyStatistics::categoryName(int id,
                                                  const QString & name,
                                                  const QString & disp) const
{
  return HTTarget::
    linkToFunction(disp.isEmpty() ? name : disp,
                   &TransactionListDialog::showMonthlyCategoryTransactions,
                   wallet->namedCategory(name),
                   account, id*12, 12);
}

//////////////////////////////////////////////////////////////////////

Statistics::Statistics(const TransactionPtrList & lst, bool topLevel)
{
  if(lst.size() > 0) 
    account = lst.first()->account;
  else
    account = NULL;

  stats << new CategorizedMonthlyStatistics(account); 
  stats << new CategorizedTrimesterStatistics(account);
  stats << new CategorizedYearlyStatistics(account);

  for(int i = 0; i < lst.size(); i++)
    for(int j = 0; j < stats.size(); j++)
      stats[j]->addTransaction(lst[i], topLevel);
}

Statistics::~Statistics()
{
  for(int i = 0; i < stats.size(); i++)
    delete stats[i];
}


QString Statistics::htmlStatistics(PeriodicCategorizedStatistics * which,
                                   int number, int maxDisplay) const
{
  QList<QStringList> columns;
  QList<int> values = which->keys();
  qSort(values);
  int target = (number > 0 ? values.last() - number : 
                values.first() - 1);
  for(int i = values.last(); i > target; i--) {
    if((*which)[i].size() == 0)
      continue;
    
    QStringList c1, c2;
    c1 << QString("<b>%1</b>").arg(which->elementName(i));
    c2 << "";
    QList<CategorizedStatistics::Item> items = 
      (*which)[i].categorize();


    c1 << which->categoryName(i, items.last().category, "Revenues");
    c2 << Transaction::formatAmount(items.last().amount);

    int rest = 0;
    for(int j = 0; j < items.size()-1; j++)
      rest += items[j].amount;
    int balance = items.last().amount + rest;

    c1 << ( balance < 0 ? "Deficit<hr/>" : "Excedent<hr/>");
    c2 << QString( balance < 0 ? 
                   "<b><font color='red'>%1</font></b><hr/>" : 
                   "<b><font color='green'>%1</font></b><hr/>").
      arg(Transaction::formatAmount(items.last().amount + rest));

    c1 << "Expenses";
    c2 << Transaction::formatAmount(rest);

    for(int j = 0; j < std::min(items.size(), maxDisplay); j++) {
      c1 << which->categoryName(i, items[j].category);
      c2 << Transaction::formatAmount(items[j].amount);
    }
    columns << c1 << c2;
  }
  QString ret = "<table>\n";
  // Won't work when there are very little
  for(int i = 0; i < maxDisplay+3; i++) {
    ret += "<tr>";
    for(int j = 0; j < columns.size(); j++) {
      if(columns[j].size() > i)
        ret += QString("<td %2 style='padding: 1px 3px;'>%1</td>").
          arg(columns[j][i]).arg( ((j % 2) == 0 ? "" : "align='right'"));
      else 
        ret += "<td %2 style='padding: 1px 3px;'></td>";
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
QString Statistics::htmlStatistics(Period per, int months, int maxDisplay) const
{
  return htmlStatistics(stats[per], months, maxDisplay);
}

