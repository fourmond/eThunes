/*
    documentspage.cc: Implementation of DocumentsPage
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
#include <documentspage.hh>
#include <transactionlistdialog.hh>

DocumentsPage::DocumentsPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  documents = cabinet->allDocuments();


  model = new DocumentsModel(&documents);
  view = new QTreeView();
  layout->addWidget(view);
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  // view->setRootIsDecorated(false);
  view->setContextMenuPolicy(Qt::CustomContextMenu);
  view->setAlternatingRowColors(true);
  updateContents();

  for(int i = 0; i < DocumentsModel::LastColumn; i++)
    view->resizeColumnToContents(i);
  // connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
  // 	  SLOT(categoriesContextMenu(const QPoint &)));
}


DocumentsPage::~DocumentsPage()
{
}

QString DocumentsPage::pageTitle()
{
  return tr("All Documents");
}

QHash<Cabinet *, DocumentsPage *> DocumentsPage::documentsPages;

DocumentsPage * DocumentsPage::getDocumentsPage(Cabinet * cabinet)
{
  if(! documentsPages.contains(cabinet))
    documentsPages[cabinet] = new DocumentsPage(cabinet);
  return documentsPages[cabinet];
}

void DocumentsPage::updateContents()
{
}
