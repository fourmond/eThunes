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
#include <linkshandler.hh>

QHash<Collection *, CollectionPage *> CollectionPage::collectionPages;

CollectionPage::CollectionPage(Collection * c) : collection(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();

  /// \todo use the links handler !
  LinksHandler::handleObject(summary);
  connect(LinksHandler::getHandler(), 
	  SIGNAL(unhandledLink(const QString &)),
	  SLOT(openURL(const QString &)));
  layout->addWidget(summary);
  connect(c->cabinet, SIGNAL(documentsChanged(Collection *)),
	  SLOT(updateContents()));

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
  /// \todo Try out displaying in several columns, though that will be
  /// fun to tune
  ///
  /// \todo Alternatively, I could setup document models of some kind,
  /// though I like the Label approach.
  QString str = tr("<b>Collection : </b>%1<p>").
    arg(collection->name);
  QHash<DocumentDefinition *, QList<Document *> > dd = 
    collection->typeDocuments();
  QHash<DocumentDefinition * , QList<Document *> >::const_iterator i = 
    dd.constBegin();
  while(i != dd.constEnd()) {
    str += QString("<h2>%1</h2><p>").arg(i.key()->definitionName());
    str += " <a href='look-matching'>Look for matching transactions</a><p>";
    for(int j = 0; j < i.value().size(); j++) {
      Document * doc = i.value()[j];
      /// \todo Use PDF logo ! (the right way...)
      str += QString("<a href='file://%1'>").arg(doc->filePath()) +
	"<img src='/usr/share/icons/hicolor/16x16/apps/adobe.pdf.png'/></a> " +
	doc->displayText();
      for(int k = 1; k < doc->attachmentsNumber(); k++)
	str += QString(" <a href='file://%1'>").arg(doc->filePath(k)) +
	  "<img src='/usr/share/icons/hicolor/16x16/apps/adobe.pdf.png'/></a>";
      str += "<a href='attach:" + doc->canonicalFileName() + 
	"'> (attach file)</a>"; /// \todo tr around.
      for(int l = 0; l < doc->links.size(); l++)
	if(doc->links[l].target) // {
	  str += " " +
	    LinksHandler::linkTo(doc->links[l].target,
				 doc->links[l].target->publicTypeName());
	//   QString link2 = 
	//     LinksHandler::linkTo(static_cast<Transaction*>(doc->links[l].target),
	// 			 "biniou");
	//   QTextStream o(stdout);
	//   o << link << endl 
	//     << link2 << endl;

	//   str += " " + link;
	// }
      str += "\n<br>\n";
    }
    i++;
  }
  
  // temporary code for testing Ruby fetching code.
  if(collection->definition->code.canFetch())
    str += "<p><a href='download'>Download new elements</a>";


  summary->setText(str);
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
  else if(str.startsWith("attach:")) {
    Document * doc = collection->cabinet->
      namedDocument(str.mid(str.indexOf(":") + 1));
    if(doc)
      promptForFileAttachment(doc);
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
    AttributeHash a;
    QList<AttributeHash> b;
    /// \todo This should be turned into a proper handling of the 
    a["login"] = QInputDialog::getText(this, tr("login"), tr("login"));
    a["passwd"] = QInputDialog::getText(this, tr("passwd"), tr("passwd"), 
					QLineEdit::Password);
    collection->definition->code.fetchNewDocuments(a,b);
  }
}

void CollectionPage::promptForFileAttachment(Document * doc)
{
  QString file = 
    QFileDialog::getOpenFileName(this, 
				 tr("Attach a file to %1").arg(doc->canonicalFileName()));
  if(file.isEmpty())
    return;
  doc->attachAuxiliaryFile(file);
  doc->bringFileIntoOwnership(-1);
  collection->cabinet->signalDocumentsChanged(collection);
}
