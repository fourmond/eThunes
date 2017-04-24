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

Budget::Budget() : amount(0), periodicity(1)
{
  
}

SerializationAccessor * Budget::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name);
  ac->addScalarAttribute("amount", &amount);
  ac->addScalarAttribute("periodicity", &periodicity);
  ac->addListAttribute("realization", &realizations);
  return ac;
}

void Budget::finishedSerializationRead()
{
  for(int i = 0; i < realizations.size(); i++)
    realizations[i].budget = this;
}

BudgetRealization * Budget::realizationForDate(const QDate & date)
{
  if(! date.isValid())
    return NULL;
  for(int i = 0; i < realizations.size(); i++) {
    if(realizations[i].contains(date))
      return &realizations[i];
  }
  realizations << BudgetRealization();
  BudgetRealization & lst = realizations.last();
  lst.amount = amount;
  lst.budget = this;

  int year = date.year();
  int month = date.month() - (date.month() % periodicity) + 1;
  lst.startDate.setDate(year, month, 1);
  lst.endDate.setDate(year, month+periodicity, 1);
  lst.endDate = lst.endDate.addDays(-1);
  return &lst;
}





//////////////////////////////////////////////////////////////////////

BudgetRealization::BudgetRealization() : budget(NULL)
{
}

SerializationAccessor * BudgetRealization::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("amount", &amount);
  ac->addScalarAttribute("start", &startDate);
  ac->addScalarAttribute("end", &endDate);
  return ac;
}

QString BudgetRealization::typeName() const
{
  return "budget-realization";
}

bool BudgetRealization::contains(const QDate & date) const
{
  return startDate <= date && date <= endDate;
}
