/**
    \file mainwin.hh
    Main window for eThunes
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

#include <actions.hh>

#include <cabinet.hh>
#include <cabinetpage.hh>

#include <logstorage.hh>

#include <navigationwidget.hh>

/// The main window
///
/// \todo handle application-wide logging (possibly saving into a real
/// log file)
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

  /// The Cabinet
  Cabinet * cabinet;

  /// The navigation widget
  NavigationWidget * navigationWidget;

  /// The dashboard
  CabinetPage * dashboard;

  /// Storage for the log messages
  LogStorage * storage;

public:
  MainWin();
  ~MainWin();

public slots:

  /// Change the title of the window according to the dirty flag
  void dirtyChanged(bool dirty);

  /// Change the title of the windows to reflect the file name
  void fileNameChanged(const QString & newName);

  /// Try to quit, but prompt for saving before if dirty.
  void tryQuit();

  /// A slot for testing purposes: loads a named definition (but does
  /// not register it !) and tries to load document from it, dumping
  /// useful data.
  void testCollectionDefinitionDocument();


  /// Fires up a LogViewer instance to watch the log.
  void displayLog();

  

};

#endif
