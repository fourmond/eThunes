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

#include <widgetwrapperdialog.hh>

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


/// Prompts for a date. Returns an invalid date on cancel.
QDate Utils::promptForDate(QWidget * parent, const QString & title,
                           const QString & label, const QDate & initial)
{
  QDateEdit * edit = new QDateEdit;
  if(initial.isValid())
    edit->setDate(initial);
  edit->setCalendarPopup(true);
  WidgetWrapperDialog * dlg =  new WidgetWrapperDialog(edit,
                                                       label);
  dlg->setWindowTitle(title);
  dlg->setAttribute(Qt::WA_DeleteOnClose, false);
  dlg->exec();                 /// @todo handle OK/Cancel
  QDate d = edit->date();
  delete dlg;
  return d;
}

QString Utils::relativePath(const QString & path, const QDir & dir)
{
  QString rp = dir.relativeFilePath(path);
  if(rp.startsWith(".."))
    return path;
  return rp;
}

QString Utils::commonSubstring(const QStringList & lst)
{
  // Find the shortest string...
  if(lst.size() < 1)
    return QString();
  int idx = 0;
  int sz = lst[0].size();
  for(int i = 1; i < lst.size(); i++) {
    const QString & s = lst[i];
    if(s.size() < sz) {
      sz = s.size();
      idx = i;
    }
  }

  QStringList l = lst;
  QString smaller = l.takeAt(idx);

  int csz = sz;
  while(sz > 0) {
    for(int i = 0; i < sz - csz; i++) {
      QString probe = smaller.mid(i, sz);
      bool found = true;
      for(int j = 0; j < l.size(); j++) {
        if(! l[j].contains(probe)) {
          found = false;
          break;
        }
      }
      if(found)
        return probe;
    }

    --sz;
  }

  return QString();
}
