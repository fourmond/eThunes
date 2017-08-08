/*
    documentspage.cc: Implementation of DocumentsPage
    Copyright 2017 by Vincent Fourmond

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
#include <document.hh>
#include <documentspage.hh>
#include <cabinet.hh>
#include <documentsmodel.hh>

DocumentsPage::DocumentsPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  model = new DocumentsModel(cabinet);
  // listView = new QListView();
  treeView = new QTreeView;
  layout->addWidget(treeView);

  treeView->setModel(model);
  treeView->setRootIndex(model->root());

  connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(treeViewContextMenu(const QPoint &)));
  treeView->setContextMenuPolicy(Qt::CustomContextMenu);

  treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

}


DocumentsPage::~DocumentsPage()
{
}

QString DocumentsPage::pageTitle()
{
  return tr("Documents");
}

QHash<Cabinet *, DocumentsPage *> DocumentsPage::documentsPages;

DocumentsPage * DocumentsPage::getDocumentsPage(Cabinet * cab)
{
  if(! documentsPages.contains(cab))
    documentsPages[cab] = new DocumentsPage(cab);
  return documentsPages[cab];
}

QList<Document*> DocumentsPage::selectedDocuments()
{
  QList<Document *> sels;
  QModelIndexList lst = treeView->selectionModel()->selectedIndexes();
  for(QModelIndex idx : lst) {
    if(! model->isDir(idx))
      sels << model->modifiableDocument(idx);
  }
  // QTextStream o(stdout);
  // for(auto a : sels)
  //   o << "Sel: " << a << ": " << (a ? a->fileName() : "(null)" ) << endl;
  // o << "-> done" << endl;
  return sels;
}

void DocumentsPage::treeViewContextMenu(const QPoint & pos)
{
  QMenu menu;

  QList<Categorizable *> targets;
  QList<Document *> docs = selectedDocuments();
  // QTextStream o(stdout);
  for(auto a : docs) {
    // o << "doc: " << a << endl;
    targets << a;
  }
  Categorizable::fillMenuWithCategorizableActions(&menu, targets);
  // o << " -> end" << endl;
  menu.exec(treeView->viewport()->mapToGlobal(pos));
}
