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

/**
   \mainpage Projects for QMoney

   There are several things I would like to do with this program.

   \section todo-banking Banking side

   First, it is important to go on with the banking part. The most
   important work on that side is:
   
   \li provide a kind of delegate class that will handle proxying raw
   account data into something viewable/editable by a ModelView
   instance.

   \li provide data persistence, through XML output (easy to diff,
   check into VCS).
   
   \li provide categories, subcategories and tags, and, following
   statistics of all kinds

   \li provide support for multiple accounts (easy, in principle)

   \section todo-admin Administration

   I want to turn this program into something more generally useful
   for performing administration tasks of households (but possibly
   small-scale businesses ?). This would be a storage place for bills,
   certificates, and all kinds of electronic documents (PDF ?). It would:

   \li retrieve automatically bills, payments and the like, using
   HTTP(S) and appropriate Regex-like parsers (with automatic
   notifications of items found).
   
   \li organise them into folders easy to browse (both in the
   underlying physical FS and in the way to present the data).

   \li This would require finding a way to make sure we don't download
   the contents twice.

   \li Maybe it would be nice to display the HTML page to the user
   when the program fails to find something on it ?

   \li This requires as well to design few various HTTP groping
   classes that will look for patterns in very specific things.

   
*/

#include <headers.hh>
#include <account.hh>
#include <ofximport.hh>
#include <account-model.hh>
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

void MainWin::setupFrame()
{
  // We want a status bar anyway...
  statusBar();
  OFXImport import = OFXImport::importFromFile("test.ofx");
  account = import.accounts[0];
  account.importTransactions(import.transactions);

  // And now, small fun:
  QTreeView * view = new QTreeView(this);
  setCentralWidget(view);

  AccountModel * model = 
    new AccountModel(account.transactions);
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
