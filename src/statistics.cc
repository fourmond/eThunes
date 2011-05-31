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

#include <linkshandler.hh>
#include <wallet.hh>
#include <account.hh>

/// @todo ignore transactions that have an "internal move" link.
void CategorizedStatistics::addTransaction(const Transaction * t)
{
  QString cat;
  // We ignored transactions flagged as internal move
  if(t->hasNamedLinks("internal move"))
    return;
  if(t->getCategory())
    cat = t->getCategory()->topLevelCategory()->name;
  else
    cat = "(uncategorized)";
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

void CategorizedMonthlyStatistics::addTransaction(const Transaction * t)
{
  (*this)[t->monthID()].addTransaction(t);
}

Statistics::Statistics(const TransactionPtrList & lst)
{
  for(int i = 0; i < lst.size(); i++)
    stats.addTransaction(lst[i]);
  if(lst.size() > 0) 
    account = lst.first()->account;
  else
    account = NULL;
}


/// @todo This just doesn't substitute for a proper widget. Here are
/// few ideas for it:
/// @li all category names should be links displaying the category
/// transactions for the corresponding month
/// @li it should provide graphics display (possibly by alternance)
/// where each data point would be very visible, with a neat tooltip
/// and a context menu for showing transactions ?
/// @li it should also provide horizontal sliding
QString Statistics::htmlStatistics(int months) const
{
  QList<QStringList> columns;
  Wallet * wallet = account->wallet;
  int target = (months > 0 ? Transaction::thisMonthID() - months : 
                account->firstMonthID() - 1);
  for(int i = Transaction::thisMonthID(); i > target; i--) {
    if(stats[i].size() == 0)
      continue;
    QStringList c1, c2;
    c1 << QString("<b>%1</b>").
      arg(LinksHandler::
          linkToMonthlyTransactions(account, i,
                                    Transaction::dateFromID(i).
                                    toString("MMM yyyy")));
    c2 << "";
    QList<CategorizedStatistics::Item> items = 
      stats[i].categorize();


    c1 << LinksHandler::linkToMonthlyCategoryTransactions(wallet->namedCategory(items.last().category), account, i, "Revenues");
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

    for(int j = 0; j < std::min(items.size(), 5); j++) {
      QString name = items[j].category;
      c1 << LinksHandler::
        linkToMonthlyCategoryTransactions(wallet->namedCategory(name),
                                          account, i, name);
      c2 << Transaction::formatAmount(items[j].amount);
    }
    columns << c1 << c2;
  }
  QString ret = "<table>\n";
  // Won't work when there are very little
  for(int i = 0; i < 8; i++) {
    ret += "<tr>";
    for(int j = 0; j < columns.size(); j++)
      ret += QString("<td %2 style='padding: 1px 3px;'>%1</td>").
        arg(columns[j][i]).arg( ((j % 2) == 0 ? "" : "align='right'"));
    ret += "</tr>\n";
  }
  ret += "</table>";
  return ret;
}

