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
#include <documentspage.hh>
#include <cabinet.hh>

DocumentsPage::DocumentsPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  model = new QFileSystemModel;
  // listView = new QListView();
  treeView = new QTreeView;
  layout->addWidget(treeView);

  treeView->setModel(model);
  treeView->setRootIndex(model->setRootPath(cabinet->baseDirectory().canonicalPath()));
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
