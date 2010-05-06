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

   \section todo-short Shorter term things to do
   
   \li It is more and more important to make sure that the objects
   communicate well about their status changes: updates of the windows
   + dirty flag for saving.

*/

#include <headers.hh>
#include <account.hh>
#include <ofximport.hh>
#include <accountmodel.hh>
#include <mainwin.hh>

#include <filterdialog.hh>

// temporary ?
#include <collection.hh>

MainWin::MainWin()
{
  cabinet = new Cabinet();
  connect(cabinet, SIGNAL(dirtyChanged(bool)), SLOT(dirtyChanged(bool)));
  fileNameChanged(QString());
  
  setupFrame();
  setupActions();
  setupMenus();

  loadSettings();
}

MainWin::~MainWin()
{
  // We save the settings
  saveSettings();
  if(cabinet)
    delete cabinet;
}

void MainWin::setupFrame()
{
  // We want a status bar anyway...
  statusBar();

  navigationWidget = new NavigationWidget();
  dashboard = new CabinetPage(cabinet);
  navigationWidget->addPage(dashboard);

  connect(dashboard, SIGNAL(filenameChanged(const QString &)),
  	  SLOT(fileNameChanged(const QString &)));

  setCentralWidget(navigationWidget);
}

void MainWin::setupActions()
{
  // Quitting...
  actions.addAction(this, "quit", tr("&Quit"),
		    this, SLOT(tryQuit()), /// \todo replace by close
					   /// and reimplement the
					   /// handler for closing
		    QKeySequence(tr("Ctrl+Q")),
		    tr("Exit from QMoney"));

  actions.addAction(this, "save", tr("&Save"),
		    dashboard, SLOT(save()),
		    QKeySequence(tr("Ctrl+S")),
		    tr("Saves the wallet"));

  actions.addAction(this, "load", tr("&Load"),
		    dashboard, SLOT(load()),
		    QKeySequence(tr("Ctrl+L")),
		    tr("Loads a new wallet"));

  actions.addAction(this, "save as", tr("Save &As"),
		    dashboard, SLOT(saveAs()),
		    QKeySequence(),
		    tr("Saves the wallet under a new name"));

  actions.addAction(this, "import", tr("&Import transactions"),
		    dashboard->walletDW, SLOT(fileImportDialog()),
		    QKeySequence(tr("Ctrl+I")),
		    tr("Imports transactions into the wallet"));

  actions.addAction(this, "manage filters", tr("&Manage filters"),
		    dashboard->walletDW, SLOT(manageFilters()),
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

  actions.addAction(this, "test definition", tr("&Test definition"),
		    this, SLOT(testCollectionDefinitionDocument()),
		    QKeySequence(tr("Ctrl+Shift+T")),
		    tr("Test definition"));

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

  QMenu * testMenu = menuBar()->addMenu(tr("Testing"));
  testMenu->addAction(actions["test definition"]);
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
  if(! dashboard->currentFileName().isEmpty())
    settings.setValue("lastfile", dashboard->currentFileName());
}


void MainWin::loadSettings()
{
  ownSettings;
  if(settings.contains("size"))
    resize(settings.value("size").toSize());
  if(settings.contains("lastfile"))
    dashboard->load(settings.value("lastfile").toString());
}

void MainWin::dirtyChanged(bool dirty)
{
  setWindowModified(dirty);
}

void MainWin::fileNameChanged(const QString & newName)
{
  if(newName.isEmpty())
    setWindowTitle(tr("QMoney[*]"));
  setWindowTitle(tr("%1[*] - QMoney").arg(QFileInfo(newName).fileName()));
}

void MainWin::tryQuit()
{
  if(isWindowModified()) {
    if(QMessageBox::question(this, tr("Quit with unsaved changes ?"),
			     tr("The wallet has unsaved changes, do you still "
				"want to quit ?"),
			     QMessageBox::Ok|QMessageBox::No, 
			     QMessageBox::No) == QMessageBox::No)
      return;
  }
  close();
}

void MainWin::testCollectionDefinitionDocument()
{
  QString type = 
    QInputDialog::getItem(this, tr("Collection definition to test"),
			  tr("Collection definition"),
			  CollectionDefinition::availableDefinitions(), 0,
			  false);
  if(type.isEmpty())
    return;
  CollectionDefinition * def = 
    CollectionDefinition::loadWithoutRegistering(type);
  if(! def)
    return;			// Though things go wrong !

  QHash<QString, QString> filters = def->documentFileFilters();
  QString filter = filters[filters.keys()[0]];

  do {
    QString file = 
      QFileDialog::getOpenFileName(this, 
				   tr("Please select a file to test"),
				   QString(),
				   QStringList(filters.keys()).join(";;"),
				   &filter);
  
    if(file.isEmpty())
      return;

    QTextStream o(stdout);
    AttributeHash contents = CollectionCode::readPDF(file);
    o << "File " << file << "'s raw attributes: " << endl;
    contents.dumpContents();
    AttributeHash outAttrs = def->code.
      parseDocumentMetaData(filters[filter], contents);
    o << endl << "Parse attributes:" << endl;
    outAttrs.dumpContents();

    DocumentDefinition * dd = def->documentDefinition(filters[filter]);
    o << "Final strings: " << endl;
    o << "Display text: " << outAttrs.formatString(dd->displayFormat) << endl;
    o << "File name: " << outAttrs.formatString(dd->fileNameFormat) << endl;
  } while(true);
}
