/*
    mainwin.cc: Main window for eThunes
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
   \mainpage Projects for eThunes

   The main things that should be done

   \section todo-mine Things to do

   \li A signalling mechanism should be written for Document and
   Transaction, and for basically anything that should be stored into
   a Qt container, which therefore cannot be a child of QObject; this
   would require setting up a shared class that would listen signals
   (with pointers to the relevant object ?)

   \li Customize the frequency at which new documents should be polled
   from master sites ?

   \li the filters dialog should move to a real page

   \li provide automatic completion for category in the filter

   \li categories could be created also from the context menu

   \li from the context menu, it should be possible to create a filter
   from a series of transactions (at least two, most probably)

   \li Write a DocumentPage that would hold only one Document; this
   will of course need access to libpoppler, at
   http://people.freedesktop.org/~aacid/docs/qt4/

   \li Write a "test filter" button to make sure not too many/too
   little transactions match the given filter.

   \li Start working on pie charts and the like ? Using a base
   category for Salaries (and not counting everything positive as
   salaries, as this wouldn't make too much sense). This mean adding a
   tag for the category that will hold salaries.

   \li Find a way to obfuscate amounts (probably from within
   formatAmount()) using a command-line switch, so I can show to
   others without actually saying too much about how much we have
   ;-)...

   \li Write some code for a transaction matcher

   \li Finish up the logger

   \li Eventually, I will have to start to write up some
   documentation...

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
		    tr("Exit from eThunes"));

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

  actions.addAction(this, "test matching", tr("Test matching transations"),
		    dashboard->walletDW, SLOT(findInternalMoves()),
		    QKeySequence(tr("Ctrl+Y")),
		    tr("Test stuff..."));

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
  fileMenu->addAction(actions["test matching"]);
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
				       "eThunes");			\
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
    setWindowTitle(tr("eThunes[*]"));
  setWindowTitle(tr("%1[*] - eThunes").arg(QFileInfo(newName).fileName()));
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
