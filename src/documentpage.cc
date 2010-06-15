/*
    documentpage.cc: Implementation of DocumentPage
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
#include <documentpage.hh>

DocumentPage::DocumentPage(Document * d) : document(d)
{

  // QVBoxLayout * layout = new QVBoxLayout(this);
  // model = new DocumentModel(wallet);
  // view = new QTreeView();
  // layout->addWidget(view);
  // view->setModel(model);
  // view->setRootIndex(model->index(0,0));
  // // view->setRootIsDecorated(false);
  // view->setContextMenuPolicy(Qt::CustomContextMenu);
  // view->setAlternatingRowColors(true);
  // updateContents();
  // connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
  // 	  SLOT(categoriesContextMenu(const QPoint &)));
}


DocumentPage::~DocumentPage()
{
}

QString DocumentPage::pageTitle()
{
  return tr("Document %1").arg(document->canonicalFileName());
}

QHash<Document *, DocumentPage *> DocumentPage::documentPages;

DocumentPage * DocumentPage::getDocumentPage(Document * doc)
{
  if(! documentPages.contains(doc))
    documentPages[doc] = new DocumentPage(doc);
  return documentPages[doc];
}

void DocumentPage::updateContents()
{
}
