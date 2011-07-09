/*
    collectionpage.cc: Implementation of CollectionPage
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
#include <collectionpage.hh>
#include <transactionlistdialog.hh>
#include <documentlistwidget.hh>

QHash<Collection *, CollectionPage *> CollectionPage::collectionPages;

static bool compareDefinitions(DocumentDefinition * a, DocumentDefinition * b)
{
  return a->name < b->name;
}


CollectionPage::CollectionPage(Collection * c) : collection(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  summary = new QLabel();

  layout->addWidget(summary);
  connect(c->cabinet, SIGNAL(documentsChanged(Collection *)),
	  SLOT(updateContents()));

  QList<DocumentDefinition *> definitions;
  for(QHash<QString, DocumentDefinition>::iterator i = 
        c->definition->documentTypes.begin(); 
      i != c->definition->documentTypes.end();
      i++)
    definitions << &i.value();

  qSort(definitions.begin(), definitions.end(), compareDefinitions);
  for(int i = 0; i < definitions.size(); i++) {
    DocumentDefinition * def = definitions[i];
    QLabel * label = new QLabel();
    label->setText(QString("<h2>%1</h2>").arg(def->definitionName()));
    layout->addWidget(label);
    documentWidgets[def] = new DocumentListWidget();
    layout->addWidget(documentWidgets[def]);
  }
    


  updateContents();
}


CollectionPage::~CollectionPage()
{
}

QString CollectionPage::pageTitle()
{
  return tr("Collection %1").arg(collection->name);
}

void CollectionPage::updateContents()
{
  QString str = tr("<b>Collection : </b>%1<p>").
    arg(collection->name);
  QHash<DocumentDefinition *, QList<Document *> > dd =
    collection->typeDocuments();
  summary->setText(str);

  for(QHash<DocumentDefinition * , QList<Document *> >::const_iterator i =
        dd.constBegin(); i != dd.constEnd(); i++)
    documentWidgets[i.key()]->showDocuments(i.value());
}

CollectionPage * CollectionPage::getCollectionPage(Collection * collection)
{
  if(! collectionPages.contains(collection))
    collectionPages[collection] = new CollectionPage(collection);
  return collectionPages[collection];
}


void CollectionPage::openURL(const QString &str)
{
  if(str.startsWith("file://")) {
    /// \todo There should be a global function providing a proxy for
    /// openUrl, to allow for local redifinition of dedicated
    /// applications, including internal viewers if and when
    /// applicable.
    QDesktopServices::openUrl(str);
  }
  else if(str.startsWith("look-matching")) {
    for(int i = 0; i < collection->documents.size(); i++) {
      Document * doc = &(collection->documents[i]);
      if(!doc->links.size())
	doc->addLink(collection->cabinet->
		     matchingTransaction(doc));
    }
    updateContents();
    /// \todo Display of what was found / log ?
  }
  else if(str == "download") {
    AttributeHash user;
    /// \todo This should be turned into a proper handling of the
    /// passwords and of multiple identities...


    user["login"] = QInputDialog::getText(this, tr("login"), tr("login"));
    user["passwd"] = QInputDialog::getText(this, tr("passwd"), tr("passwd"),
					   QLineEdit::Password);
    collection->fetchNewDocumentsForUser(user);
  }
}

