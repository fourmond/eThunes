/*
    actions.cc: a hash storing actions conveniently
    Copyright 2008 by Vincent Fourmond

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
#include <actions.hh>

QAction * ActionsHash::addAction(QString codename, QAction * action, 
				 const QKeySequence & shortcut,
				 QString tooltip)
{
  action->setShortcut(shortcut);
  action->setToolTip(tooltip);
  if(contains(codename) && value(codename))
    delete value(codename);
  insert(codename,action);
  return action;
}

QAction * ActionsHash::addAction(QObject * owner,
				 QString codename, QString text,
				 const QObject * receiver, const char * member,
				 const QKeySequence & shortcut,
				 QString tooltip)
{
  QAction * action = new QAction(text, owner);
  if(receiver)
    receiver->connect(action, SIGNAL(triggered()), member);
  return addAction(codename, action, shortcut, tooltip);
}
