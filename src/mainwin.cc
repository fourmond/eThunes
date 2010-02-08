/*
    mainwin.cc: Main window for QMoney
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
#include <account.hh>
#include <ofximport.hh>
#include <transaction-list-model.hh>
#include <mainwin.hh>

MainWin::MainWin()
{
  setupFrame();
  setupActions();
  setupMenus();

  loadSettings();
}

MainWin::~MainWin()
{
  // We save the settings
  saveSettings();
}

OFXImport import;

void MainWin::setupFrame()
{
  // We want a status bar anyway...
  statusBar();

  // And now, small fun:
  QTreeView * view = new QTreeView(this);
  setCentralWidget(view);
  
  /// \todo: here, I beg for segfaults (the import's accounts go out
  /// of scope when the function finishes) !
  ///
  /// \todo maybe use a static hash for storing accounts based on
  /// their unique names ?
  import = OFXImport::importFromFile("test.ofx");
  QList<Transaction> list = import.transactions;

  TransactionListModel * model = new TransactionListModel(list);
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
}

void MainWin::setupActions()
{
  // Quitting...
  actions.addAction(this, "quit", tr("&Quit"),
		    this, SLOT(close()),
		    QKeySequence(tr("Ctrl+Q")),
		    tr("Exit from QMoney"));
}

void MainWin::setupMenus()
{
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(actions["quit"]);
}

// This sounds ridiculous, but I'll have to use a macro, as the copy
// constructor for QSettings is *private*.

#define ownSettings QSettings settings("tanyaivinco.homelinux.org",	\
				       "QMoney");			\
  settings.beginGroup("mainwin");

void MainWin::saveSettings()
{
  ownSettings;
  settings.setValue("size", size());
}


void MainWin::loadSettings()
{
  ownSettings;
  if(settings.contains("size"))
    resize(settings.value("size").toSize());
}
