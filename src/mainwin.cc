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
   
   \li provide categories, subcategories and tags, and, following
   statistics of all kinds

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
#include <accountmodel.hh>
#include <mainwin.hh>

#include <filterdialog.hh>

MainWin::MainWin()
{
  /// \todo here change !
  wallet = new Wallet();
  
  setupFrame();
  setupActions();
  setupMenus();

  loadSettings();
}

MainWin::~MainWin()
{
  // We save the settings
  saveSettings();
  if(wallet)
    delete wallet;
}

void MainWin::setupFrame()
{
  // We want a status bar anyway...
  statusBar();
  // OFXImport import = OFXImport::importFromFile("test.ofx");
  // account = import.accounts[0];
  // account.importTransactions(import.transactions);

  // // And now, small fun:
  // QTreeView * view = new QTreeView(this);
  // setCentralWidget(view);

  // AccountModel * model = 
  //   new AccountModel(account.transactions);
  // view->setModel(model);
  // view->setRootIndex(model->index(0,0));

  walletDW = new WalletDW(wallet);

  navigationWidget = new NavigationWidget();
  /// \todo eventually navigationWidget should only see NavigationPage
  /// objects.
  navigationWidget->addTab(walletDW, tr("Dashboard"));

  setCentralWidget(navigationWidget);
}

void MainWin::setupActions()
{
  // Quitting...
  actions.addAction(this, "quit", tr("&Quit"),
		    this, SLOT(close()),
		    QKeySequence(tr("Ctrl+Q")),
		    tr("Exit from QMoney"));

  actions.addAction(this, "save", tr("&Save"),
		    walletDW, SLOT(save()),
		    QKeySequence(tr("Ctrl+S")),
		    tr("Saves the wallet"));

  actions.addAction(this, "load", tr("&Load"),
		    walletDW, SLOT(load()),
		    QKeySequence(tr("Ctrl+L")),
		    tr("Loads a new wallet"));

  actions.addAction(this, "save as", tr("Save &As"),
		    walletDW, SLOT(saveAs()),
		    QKeySequence(),
		    tr("Saves the wallet under a new name"));

  actions.addAction(this, "import", tr("&Import transactions"),
		    walletDW, SLOT(fileImportDialog()),
		    QKeySequence(tr("Ctrl+I")),
		    tr("Imports transactions into the wallet"));

  actions.addAction(this, "manage filters", tr("&Manage filters"),
		    walletDW, SLOT(manageFilters()),
		    QKeySequence(),
		    tr("Creates and edit filters"));

  // Tab-related actions
  actions.addAction(this, "next tab", tr("&Next tab"),
		    navigationWidget, SLOT(switchToNextTab()),
		    QKeySequence(Qt::CTRL + Qt::Key_Tab),
		    tr("Switches to next tab"));

  actions.addAction(this, "previous tab", tr("&Previous tab"),
		    navigationWidget, SLOT(switchToPreviousTab()),
		    QKeySequence(Qt::CTRL + Qt::Key_Backtab),
		    tr("Switches to previous tab"));

  /// \todo For some reason, the two above actions are triggered even
  /// when absent of the menus, while this one isn't.
  actions.addAction(this, "close tab", tr("&Close tab"),
		    navigationWidget, SLOT(closeCurrentTab()),
		    QKeySequence(tr("Ctrl+W")),
		    tr("Closes current tab"));
  

}

void MainWin::setupMenus()
{
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(actions["load"]);
  fileMenu->addAction(actions["save"]);
  fileMenu->addAction(actions["save as"]);
  fileMenu->addSeparator();
  fileMenu->addAction(actions["import"]);
  fileMenu->addAction(actions["manage filters"]);
  fileMenu->addSeparator();
  fileMenu->addAction(actions["quit"]);

  QMenu * tabMenu = menuBar()->addMenu(tr("&Tabs"));
  tabMenu->addAction(actions["close tab"]);
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
  if(! walletDW->currentFileName().isEmpty())
    settings.setValue("lastfile", walletDW->currentFileName());
}


void MainWin::loadSettings()
{
  ownSettings;
  if(settings.contains("size"))
    resize(settings.value("size").toSize());
  if(settings.contains("lastfile"))
    walletDW->load(settings.value("lastfile").toString());
}
