/** 
    \file mainwin.hh 
    Main window for QMoney
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

#ifndef __MAINWIN_HH
#define __MAINWIN_HH

#include <account.hh>
#include <actions.hh>

class MainWin : public QMainWindow {

  Q_OBJECT;

  /// Sets up the actions usable within this window
  void setupActions();
  void setupMenus();
  void setupFrame();

  // yet unimplemented...
  void loadSettings();
  void saveSettings();

  /// A hash storing all actions, with reasonable code-like
  /// names.
  ActionsHash actions;

  Account account;

public:
  MainWin();
  ~MainWin();
  
};

#endif
