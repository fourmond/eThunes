/*
  utils.cc: implementation of the utility functions
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
#include <utils.hh>

QList<QDate> Utils::monthList(const QDate & begin, const QDate & end)
{
  QList<QDate> ret;
  QDate cur = firstDay(begin);
  QDate e = lastDay(cur);
  QDate final = lastDay(end);

  while(e <= final) {
    ret.append(cur);
    cur = e.addDays(1);
    e = Utils::lastDay(cur);
  }

  return ret;
}

QDate Utils::promptForMonth(QWidget * parent, const QString & title,
                            const QString & label, const QDate & begin, 
                            const QDate & end, 
                            const QString & format)
{
  QList<QDate> months = monthList(begin, end);
  QStringList s;
  for(int i = 0; i < months.size(); i++)
    s.append(months[i].toString(format));

  bool ok = false;
  QString which = 
    QInputDialog::getItem(parent, title, label,
                          s, 0, false, 
                          &ok);
  if(! ok)
    return QDate();

  int idx = s.indexOf(which);
  if(idx < 0)
    return QDate();
  
  return months[idx];
}

/// @todo Provide additional arguments for the formatting (short or
/// long name ?)
QString Utils::monthName(const QDate & month, bool longFormat)
{
  QString str = month.toString(longFormat ? "MMMM yyyy" : 
                               "MMM yy");
  return Utils::capitalize(str);
}
