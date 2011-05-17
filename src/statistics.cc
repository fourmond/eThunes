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

/// @todo ignore transactions that have an "internal move" link.
void CategorizedStatistics::addTransaction(const Transaction * t)
{
  QString cat;
  if(t->category)
    cat = t->category->topLevelCategory()->name;
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
}


QString Statistics::htmlStatistics(int months) const
{
  QList<QStringList> columns;
  for(int i = Transaction::thisMonthID(); 
      i > Transaction::thisMonthID() - months; i--) {
    QStringList column;
    column << QString("<b>%1</b>").
      arg(Transaction::dateFromID(i).toString("MMM yyyy"));
    QList<CategorizedStatistics::Item> items = 
      stats[i].categorize();
    column << QString("Revenues: %1").
      arg(Transaction::formatAmount(items.last().amount));
    int rest = 0;
    for(int j = 0; j < items.size()-1; j++)
      rest += items[j].amount;
    column << QString("Expenses: %1").
      arg(Transaction::formatAmount(rest));

    for(int j = 0; j < std::min(items.size(), 4); j++)
      column << QString("%1: %2").
        arg(items[j].category).arg(Transaction::formatAmount(items[j].amount));
    columns << column;
  }
  QString ret = "<table>\n";
  // Won't work when there are very little
  for(int i = 0; i < 6; i++) {
    ret += "<tr>";
    for(int j = 0; j < columns.size(); j++)
      ret += QString("<td>%1</td>").arg(columns[j][i]);
    ret += "</tr>\n";
  }
  ret += "</table>";
  return ret;
}

