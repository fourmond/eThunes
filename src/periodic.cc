/*
    periodic.cc: The Periodic class and Period
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
#include <periodic.hh>

#include <serialization.hh>
#include <utils.hh>

Period::Period()
{
}

Period::Period(const QDate & date, int daysBefore, int daysAfter) :
  startDate(date.addDays(-daysBefore)), endDate(date.addDays(daysAfter))
{
}

bool Period::contains(const QDate & date) const
{
  return (startDate <= date) && (date <= endDate);
}

bool Period::isValid() const
{
  if(! (startDate.isValid() && endDate.isValid()))
    return false;
  return startDate <= endDate;
}

uint qHash(const Period & p, uint seed)
{
  return qHash(p.startDate) ^ qHash(p.endDate) ^ seed;
}

bool Period::operator<(const Period & other) const
{
  return startDate < other.startDate;
}

bool Period::operator==(const Period & other) const
{
  return startDate == other.startDate &&
    endDate == other.endDate;
}

int Period::months() const
{
  return endDate.month() - startDate.month() + 1 +
    12 * (endDate.year() - startDate.year());
}

Period Period::intersect(const Period & other) const
{
  Period rv;
  rv.startDate = std::max(other.startDate, startDate);
  rv.endDate = std::min(other.endDate, endDate);
  return rv;
}

Period Period::unite(const Period & other) const
{
  Period rv;
  rv.startDate = std::min(other.startDate, startDate);
  rv.endDate = std::max(other.endDate, endDate);
  return rv;
}

Period Period::currentYear()
{
  QDate cur = QDate::currentDate();
  return year(cur.year());
}

Period Period::year(int y)
{
  Period rv;
  rv.startDate.setDate(y, 1, 1);
  rv.endDate.setDate(y, 12, 31);
  return rv;
}

Period Period::month(int year, int month)
{
  Period rv;
  rv.startDate.setDate(year, month, 1);
  rv.endDate = rv.startDate.addMonths(1).addDays(-1);
  return rv;
}

template <> void 
SerializationAccessor::addScalarAttribute(const QString & name,
                                          Period * target,
                                          bool isXMLAttribute)
{
  addScalarAttribute(name + "-start", &(target->startDate), isXMLAttribute);
  addScalarAttribute(name + "-end", &(target->endDate), isXMLAttribute);
}

//////////////////////////////////////////////////////////////////////

Periodic::Periodic(int m, int fm) :
  months(m), firstMonth(fm)
{
}

Period Periodic::periodForDate(const QDate & date) const
{
  Period rv;
  if(! date.isValid())
    return rv; // invalid period
  int year = date.year();
  int month = date.month();
  if(month < firstMonth) {
    year -= 1;
    month += 12;
  }
  
  month -= (month - firstMonth) % months;
  rv.startDate.setDate(year, month, 1);
  rv.endDate = rv.startDate.addMonths(months).addDays(-1);
  return rv;
}

Period Periodic::nextPeriod(const Period & period) const
{
  Period rv;
  if(! period.isValid())
    return rv;
  rv.startDate = period.endDate.addDays(1);
  rv.endDate = rv.startDate.addMonths(months).addDays(-1);
  return rv;
}

Period Periodic::previousPeriod(const Period & period) const
{
  Period rv;
  if(! period.isValid())
    return rv;
  rv.endDate = period.startDate.addDays(-1);
  rv.startDate = period.startDate.addMonths(-months);
  return rv;
}

QString Periodic::periodName(const Period & period, bool lg) const
{
  if(months == 1)
    return Utils::monthName(period.startDate, lg);
  if(months == 12 && firstMonth == 1) // truly yearly
    return QString("%1").arg(period.startDate.year());
  return QString("%1 - %2").
    arg(Utils::monthName(period.startDate, lg)).
    arg(Utils::monthName(period.endDate, lg));
}


const Periodic Periodic::monthly(1, 1);
const Periodic Periodic::trimester(3, 1);
const Periodic Periodic::yearly(12, 1);
const Periodic Periodic::schoolYear(12, 9);

template <> void 
SerializationAccessor::addScalarAttribute(const QString & name,
                                          Periodic * target,
                                          bool isXMLAttribute)
{
  addScalarAttribute(name + "-months", &(target->months), isXMLAttribute);
  addScalarAttribute(name + "-first", &(target->firstMonth), isXMLAttribute);
}
