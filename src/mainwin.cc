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

   This list is mostly in priority order.

   \li \b Signals 
   A signalling mechanism should be written for Document and
   Transaction, and for basically anything that should be stored into
   a Qt container, which therefore cannot be a child of
   QObject. Probably the best way to achieve that is to use a single
   Object for signalling (there won't be too many receivers), and send
   the pointer to the element that changed -- and possibly its ID
   too. That could either be yet another base class, or be integrated
   into the Serialization framework. (because the purpose is
   eventually the same, isn't it ?)

   \li \b Budgets
   It should be possible to setup budgets, ie money that either we
   know we'll spend later on, or simply a prevision of how much should
   be allocated to something. Budgets should pick up automatically
   (based on categories, tags, possibly free searches) the
   corresponding transactions and provide planning information (how
   much left to spend for that, how much left that isn't included in
   budgeting...). The main thing to get for now is the idea of
   periodicity behind. And this is a rather subtle one. 1-month
   budget, 2-month, 3-month, a year ? Planning necessarily means
   setting a time frame, but not all budgets should have the same time
   frame, making it probably more difficult ? 

   \li \b Searches 
   Implement advanced searches, à la find. Possibly a free-text search
   expression, in the spirit of category:stuff and amount:... ? (just
   better, though).

   \li \b Dashboard 
   Dashboard should be fully customizable. For now,
   the current HVBox approach is just enough to get work done, but
   isn't nice. Many items could possibly be added there, from the
   simple summary of the accounts until documents, graphs, and so
   on. A framework will have to be designed to support that.

   \li \b Collection \b page 
   The page displaying a collection of documents isn't presented nice
   for now, and it will not scale well. Instead of using a QLabel, I
   should use a set of QListView (one for each document type) in icon
   mode (most probably), separated by a Label (or such) showing the
   corresponding document type. All these should be collapsible on the
   QLabel level (which probably shouldn't be a QLabel, then
   ?). Another thing: it would be neat to add a bottom/side panel to
   this page to display all the information available on a given
   document. Possibly, links to documents could direct to the
   collection page with the target document selected ? (and that would
   lift the requirement for a full poppler view of the things). From
   this side view, it would be interesting to provide a way to reparse
   the document ? Another thing: most probably, to get the links right,
   I should use a persistent editor for all objects, like for the links in
   the transaction view.

   \li <b>Yet again grand ideas</b> it would be nice to replace
   QInputDialog-based user interaction with more discreet frameless
   dialog boxes, almost modal: a click out would mean cancel.

   \li \b Plugins It would be great to add fully customizable plugins
   in the form of Ruby code. An exemple would be for instance for an
   assistante maternelle (as we say in french) to track how much time
   each child is received each day, how much she should be paid and
   little help with the fisc too. That requires a little bit of
   thinking as far as the structure is concerned. It could support
   following loans too.

   \li <b>Document "type safety"</b> 
   For now, there is no guarantee that a document is "correct". A
   simple things would be to add a list of required fields to a
   DocumentDefinition, and check that those are present and of the
   correct type (optionnally). Optionnallytoo, for some collections,
   it may make sense to prompt the user for the missing fields.

   \li Customize the frequency at which new documents should be polled
   from master sites ? (from the identities/password saving ?)

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

   \li Finish up the logger

   \li Eventually, I will have to start to write up some
   documentation...

   \li <b>Plugin development files</b> With the development of
   plugins, it will become necessary to have a clearly-defined
   development interface for the plugins, though for now, everything
   is about fine.
*/

#include <headers.hh>
#include <account.hh>
#include <ofximport.hh>
#include <accountmodel.hh>
#include <mainwin.hh>

#include <ootest.hh>

#include <logviewer.hh>

#include <filterdialog.hh>

// temporary ?
#include <collection.hh>

static const char * translationCopyrightString = 
  QT_TRANSLATE_NOOP("Copyright Notice",
                    "The original English localization of eThunes is\n"
             "Copyright 2008-2011 by Vincent Fourmond\n\n"
             "Localization strings are excerpts from the source code\n"
             "and are therefore licensed under the GNU General Public License\n"
             "version 2 or any later version at your options (see source code).\n\n"
             "Translators are reminded that translation are considered a\n"
             "derived work, and are therefore also subject to the GNU\n"
             "General Public License, as the source code.\n"
             );

MainWin::MainWin()
{
  storage = new LogStorage();
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

  actions.addAction(this, "find internal", tr("Find Internal Moves"),
		    dashboard->walletDW, SLOT(findInternalMoves()),
		    QKeySequence(),
		    tr("Find internal moves between accounts"));

  actions.addAction(this, "find internal permissive", tr("Find Internal Moves (permissive)"),
		    dashboard->walletDW, SLOT(findInternalMovesPermissive()),
		    QKeySequence(),
		    tr("Find internal moves between accounts "
                       "(more permissive than the other one)"));


  actions.addAction(this, "manage filters", tr("&Manage filters"),
		    dashboard->walletDW, SLOT(manageFilters()),
		    QKeySequence(),
		    tr("Creates and edit filters"));

  actions.addAction(this, "view log", tr("View log"),
		    this, SLOT(displayLog()),
		    QKeySequence(),
		    tr("View log messages, useful to know what "
		       "is really happenning"));

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
  fileMenu->addAction(actions["find internal"]);
  fileMenu->addAction(actions["find internal permissive"]);
  fileMenu->addAction(actions["manage filters"]);
  fileMenu->addAction(actions["view log"]);
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

void MainWin::displayLog()
{
  // LogViewer * viewer = new LogViewer(storage);
  // viewer->show();
  OOTest::test(&cabinet->wallet.accounts[0].transactions);
}
