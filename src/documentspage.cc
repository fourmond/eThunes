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
#include <doctype.hh>

#include <atomictransaction.hh>
#include <documentwidget.hh>
#include <accountmodel.hh>

DocumentsPage::DocumentsPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  splitter = new QSplitter(Qt::Vertical, this);
  layout->addWidget(splitter);

  model = new DocumentsModel(cabinet);
  // listView = new QListView();
  treeView = new QTreeView;
  splitter->addWidget(treeView);

  treeView->setModel(model);
  treeView->setRootIndex(model->root());

  treeView->setItemDelegateForColumn(model->nativeColumns +
                                     DocumentsModel::CategoryColumn,
                                     new AccountItemDelegate(&cabinet->wallet));

  
  treeView->setAlternatingRowColors(true);

  connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(treeViewContextMenu(const QPoint &)));

  
  treeView->setContextMenuPolicy(Qt::CustomContextMenu);

  treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(treeView, SIGNAL(activated(const QModelIndex &)),
	  SLOT(onDocumentActivated(const QModelIndex &)));

  connect(treeView->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
          SLOT(onCurrentDocumentChanged(const QModelIndex &,const QModelIndex &)));
  

  documentWidget = new DocumentWidget(cabinet);
  
  splitter->addWidget(documentWidget);

}


DocumentsPage::~DocumentsPage()
{
}

AtomicTransaction * DocumentsPage::findMatchingTransaction(const Document * doc)
{
  QTextStream o(stdout);
  Period p = doc->relevantDateRange();
  o << "Looking for transactions for " << doc 
    << "\n -> " << p.startDate.toString()
    << " -> " << p.endDate.toString()
    << "\n ---- " << doc->docType() <<  endl;
  
  if(! p.isValid())
    return NULL;
  TransactionPtrList trs = cabinet->wallet.transactionsForPeriod(p);
  AtomicTransaction * rv = NULL;
  int score = 0;
  for(AtomicTransaction * t : trs) {
    int s = doc->scoreForTransaction(t);
    if(s > score) {
      score = s;
      rv = t;
    }
  }

  o << " -> found: " << rv << endl;
  return rv;
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
  QSet<Document*> alreadyIn;
  for(QModelIndex idx : lst) {
    if(! model->isDir(idx)) {
      Document * doc = model->modifiableDocument(idx);
      if(! alreadyIn.contains(doc)) {
        alreadyIn.insert(doc);
        sels << doc;
      }
    }
  }
  // QTextStream o(stdout);
  // for(auto a : sels)
  //   o << "Sel: " << a << ": " << (a ? a->fileName() : "(null)" ) << endl;
  // o << "-> done" << endl;
  return sels;
}

static void fillWithDocTypes(QMenu * menu,   QList<Document *> docs) {
  QMenu * subMenu = new QMenu(QObject::tr("Document type"));
  QAction * a = new QAction(QObject::tr("Autodetect"));
  QObject::connect(a, &QAction::triggered, [docs](bool) {
      for(Document * doc : docs)
        doc->autoDetectDocType();
    }
    );
  subMenu->addAction(a);
  subMenu->addSeparator();

  /// @todo Add common document types directly here !

  QHash<Collection *, QList<DocType *> > dts =
    DocType::docTypesByCollection();
  for(Collection * c : dts.keys()) {
    QMenu * s2 = new QMenu(c ? c->name() : "Uncategorized");
    for(DocType * dt : dts[c]) {
      a = new QAction(dt->name());
      QObject::connect(a, &QAction::triggered, [docs, dt](bool) {
          for(Document * doc : docs)
            doc->setDocType(dt);
        }
        );
      s2->addAction(a);
    }
    subMenu->addMenu(s2);
  }
  menu->addMenu(subMenu);
}


void DocumentsPage::treeViewContextMenu(const QPoint & pos)
{
  QMenu menu;

  QList<Categorizable *> targets;
  QList<Document *> docs = selectedDocuments();
  for(auto a : docs) {
    targets << a;
  }
  Categorizable::fillMenuWithCategorizableActions(&menu, targets);

  

  menu.addSeparator();
  // Now fill with types !
  fillWithDocTypes(&menu, docs);

  menu.addSeparator();

  QModelIndex idx = treeView->indexAt(pos);
  if(! model->isDir(idx)) {
    Document * doc = model->modifiableDocument(idx);
    doc->fillMenuWithLinkableActions(&menu);
  }


  menu.addSeparator();

  QAction * a = new QAction(tr("Find matching transactions"));
  QObject::connect(a, &QAction::triggered, [this, docs](bool) {
      for(Document * d : docs) {
        AtomicTransaction * t = findMatchingTransaction(d);
        if(t)
          t->addLink(d, "document");
      }
    }
    );
  menu.addAction(a);

  a = new QAction(tr("Rename"));
  QObject::connect(a, &QAction::triggered, [this, docs](bool) {
      for(Document * d : docs) {
        QString nn =
          QInputDialog::getText(this, tr("Rename file"),
                                tr("New name"), QLineEdit::Normal,
                                d->fileName());
        if(! nn.isEmpty() && nn != d->fileName())
          cabinet->documents.renameDocument(d,nn);
      }
    }
    );
  menu.addAction(a);

  menu.exec(treeView->viewport()->mapToGlobal(pos));

}


void DocumentsPage::onCurrentDocumentChanged(const QModelIndex &current,
                                             const QModelIndex &)
{
  documentWidget->showDocument(model->filePath(current));
}

void DocumentsPage::onDocumentActivated(const QModelIndex & index)
{
  QString s = model->absoluteFilePath(index);
  if(s.isEmpty())
    return;
  QFileInfo info(s);
  if(info.isDir())
    return;
  QDesktopServices::openUrl(QUrl::fromLocalFile(s));
}
