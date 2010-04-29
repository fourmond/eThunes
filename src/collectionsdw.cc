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

CollectionsDW::CollectionsDW(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);
  connect(summary, SIGNAL(linkActivated(const QString &)), 
	  SLOT(showURL(const QString &)));

  // connect(, SIGNAL(accountsChanged()),
  // 	  SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

void CollectionsDW::updateSummary()
{
  QString text = QString("<strong>") + tr("Collections") + "</strong>\n<p>";
  QString cellStyle = " style='padding-right: 20px'";

  text += "<a href='new-collection'>Add new collection</a><p>\n";
  /// \todo Maybe the facility for building up tables should end up
  /// somewhere as global utilities ?
  text += "<table>\n";
  // text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
  //   arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < cabinet->collections.size(); i++) {
    Collection * c = &cabinet->collections[i];
    text += QString("<tr><td" + cellStyle +"><a href='collection:%1'>").
      arg(i) + QString("%1</a></td><td align='right'>%2</td>"
		       "<td><a href='add-collection:%3'>(add)</a></td></tr>\n").
      arg(c->name).arg(c->documents.size()).arg(i);
  }
  text += "<tr></tr>";
  text += "</table>\n";
  
  summary->setText(text);
}

CollectionsDW::~CollectionsDW()
{
  
}

void CollectionsDW::showURL(const QString & link)
{
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
  cabinet->collections.append(Collection());
  cabinet->collections.last().name = name;
  cabinet->collections.last().definition = 
    CollectionDefinition::namedDefinition(type);
  updateSummary();
}

void CollectionsDW::addDocumentsDialog(Collection * collection)
{
  if(!collection)
    return;
  QHash<QString, QString> filters = 
    collection->definition->documentFileFilters();
  QString filter = filters[filters.keys()[0]];
  QStringList files = 
    QFileDialog::getOpenFileNames(this, 
				  tr("Please select files to import into %1").
				  arg(collection->name),
				  QString(),
				  QStringList(filters.keys()).join(";;"),
				  &filter);
  for(int i = 0; i < files.size(); i++)
    collection->importFile(filters[filter], files[i]);
  if(files.size() > 0)
    updateSummary();
}

