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

#include <htlabel.hh>
#include <httarget-templates.hh>
#include <identitiessafe.hh>

QHash<Collection *, CollectionPage *> CollectionPage::collectionPages;

static bool compareDefinitions(DocumentDefinition * a, DocumentDefinition * b)
{
  return a->getName() < b->getName();
}


CollectionPage::CollectionPage(Collection * c) : collection(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  summary = new HTLabel;

  layout->addWidget(summary);
  connect(c->cabinet, SIGNAL(documentsChanged(Collection *)),
	  SLOT(updateContents()));

  QList<DocumentDefinition *> definitions;
  for(QHash<QString, DocumentDefinition*>::iterator i = 
        c->definition->documentTypes.begin(); 
      i != c->definition->documentTypes.end();
      i++)
    definitions << i.value();

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
  str += HTTarget::
    linkToMember("(find matching transactions) ",
                 this,
                 &CollectionPage::lookupMatchingTransactions);
  str += HTTarget::
    linkToMember("(reparse documents) ",
                 this,
                 &CollectionPage::reparseDocuments);
  if(collection->definition->canFetch()) 
    str += HTTarget::
      linkToMember("(try download) ",
                   this,
                   &CollectionPage::tryDownload);

  str += HTTarget::
    linkToMember("(checkup)",
                 this,
                 &CollectionPage::fileCheckup);
                 
  summary->setText(str);


  QHash<DocumentDefinition *, QList<Document *> > dd =
    collection->typeDocuments();
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


void CollectionPage::tryDownload()
{
  AttributeHash user = IdentitiesSafe::getCredentialsForCollection(collection);
    collection->fetchNewDocumentsForUser(user);
}

void CollectionPage::lookupMatchingTransactions()
{
  for(int i = 0; i < collection->documents.size(); i++) {
    Document * doc = &(collection->documents[i]);
    if(!doc->links.size())
      doc->addLink(collection->cabinet->
                   matchingTransaction(doc));
  }
  updateContents();
}

void CollectionPage::reparseDocuments()
{
  QList<Document *> docs = collection->allDocuments();
  for(int i = 0; i < docs.size(); i++)
    docs[i]->reparseDocument();
}

void CollectionPage::fileCheckup()
{
  QList<Document *> docs = collection->allDocuments();

  QTextStream o(stdout);
  for(int i = 0; i < docs.size(); i++) {
    Document * doc = docs[i];
    o << "File: " << doc->filePath();
    if(doc->isFileCanonical(0))
      o << " -> canonical";
    else
      o << " -> " << doc->canonicalFilePath();
    o << endl;
  }
}
