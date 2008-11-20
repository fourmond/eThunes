/** 
    \file actions.hh a hash storing actions conveniently
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

#ifndef __ACTIONS_HH
#define __ACTIONS_HH

class ActionsHash : public QHash<QString, QAction *> {
public:
  /// Adds an action with the given attributes
  QAction * addAction(QObject * owner,
		      QString codename, QString text,
		      const QObject * receiver, const char * member,
		      const QKeySequence & shortcut = 0,
		      QString tooltip = QString());
  
  QAction * addAction(QString codename, QAction * action, 
		      const QKeySequence & shortcut = 0,
		      QString tooltip = QString());
};

#endif
