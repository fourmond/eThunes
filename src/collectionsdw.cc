/*
    collectionsdw.cc: Implementation of WalletDW
    Copyright 2010 by Vincent Fourmond

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
#include <collectionsdw.hh>

#include <navigationwidget.hh>
#include <collectionpage.hh>
#include <documentspage.hh>
#include <htlabel.hh>
#include <httarget-templates.hh>


CollectionsDW::CollectionsDW(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);
  // connect(summary, SIGNAL(linkActivated(const QString &)),
  //         SLOT(showURL(const QString &)));

  connect(cabinet, SIGNAL(collectionsPossiblyChanged()),
  	  SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

void CollectionsDW::updateSummary()
{
  QString text = QString("<strong>") + tr("Collections") + "</strong>\n<p>";
  QString cellStyle = " style='padding-right: 20px'";

  text += HTTarget::linkToMember("(add new collection)",
                                 this, &CollectionsDW::addCollectionDialog);
  text += "<p>\n";

  /// \todo Maybe the facility for building up tables should end up
  /// somewhere as global utilities ?
  text += "<table>\n";
  // text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
  //   arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < cabinet->collections.size(); i++) {
    Collection * c = &cabinet->collections[i];
    text += QString("<tr><td" + cellStyle +">%1</td>").
      arg(HTTarget::linkTo(c->name, c)) + QString("<td align='right'>%2</td>"
		       "<td><a href='add-collection:%3'>(add)</a></td></tr>\n").
      arg(c->documents.size()).arg(i);
  }
  text += "<tr></tr>";
  text += "</table><p>\n";

  text += "<a href='all-documents'>See all documents</a><p>\n";

  summary->setText(text);
}

CollectionsDW::~CollectionsDW()
{

}

void CollectionsDW::showURL(const QString & link)
{
  /// \todo There should be a dialog box proposing to override the
  /// file name format for all the Collection objects. This could be
  /// called "customize file names": it would provide a choice of
  /// either a default value or a use-edited one.
  QStringList l = link.split(':');
  QString & id = l[0];
  if(id == "new-collection") {
    addCollectionDialog();
  }
  if(id == "add-collection") {
    Collection * coll = &cabinet->collections[l[1].toInt()];
    addDocumentsDialog(coll);
  }
  NavigationPage * page = NULL;
  if(id == "collection") {
    page = CollectionPage::getCollectionPage(&cabinet->
					     collections[l[1].toInt()]);
  }
  if(id == "all-documents") {
    page = DocumentsPage::getDocumentsPage(cabinet);
  }
  // else if(id == "categories") {
  //   page = CategoryPage::getCategoryPage(wallet);
  // }
  // else if(id == "filters") {
  //   manageFilters(); /// \todo shouldn't this be a page too ?
  // }
  if(page)
    NavigationWidget::gotoPage(page);
}


void CollectionsDW::addCollectionDialog()
{
  /// \todo this should be a proper dialog box, with both selections
  /// at the same time and a decent display of the things.
  QString type =
    QInputDialog::getItem(this, tr("Choose collection type"),
			  tr("Choose collection type"),
			  CollectionDefinition::availableDefinitions(), 0,
			  false);
  if(type.isEmpty())
    return;
  QString name =
    QInputDialog::getText(this, tr("Name of the new collection"),
			  tr("Please input new collection name"));
  if(name.isEmpty())
    return;
  cabinet->addNewCollection(name,type);
  updateSummary();
}

void CollectionsDW::addDocumentsDialog(Collection * collection)
{
  if(!collection)
    return;
  QHash<QString, QString> filters =
    collection->definition->documentFileFilters();
  QString filter = filters[filters.keys()[0]];

  /// \todo To do this properly, it is possible to fire up a custom
  /// QFileDialog box, with a match document type -> filter and
  /// appropriate changing of labels when the user chooses different
  /// files ? Use custom labels.
  ///
  /// It probably isn't necessary.

  QStringList files =
    QFileDialog::getOpenFileNames(this,
				  tr("Please select files to import into %1").
				  arg(collection->name),
				  QString(),
				  QStringList(filters.keys()).join(";;"),
				  &filter);
  QTextStream o(stderr);
  for(int i = 0; i < files.size(); i++)
    if( ! collection->importFile(filters[filter], files[i]))
      o << "Could not import file " << files[i] << endl;

  if(files.size() > 0)
    updateSummary();
}

