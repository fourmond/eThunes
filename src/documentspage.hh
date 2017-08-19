/**
    \file documentspage.hh
    A page displaying informations about the documentss
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

#ifndef __DOCUMENTSPAGE_HH
#define __DOCUMENTSPAGE_HH


#include <navigationpage.hh>

class Cabinet;
class DocumentsModel;
class Document;
class DocumentWidget;

/// This NavigationPage displays the informations about documentss
class DocumentsPage : public NavigationPage {

  Q_OBJECT;

protected:

  /// A correspondance Wallet* -> BudgetPage
  static QHash<Cabinet *, DocumentsPage *> documentsPages;

  Cabinet * cabinet;

  DocumentsModel * model;

  /// The vertical splitted
  QSplitter * splitter;

  /// The tree view to display documents
  QTreeView * treeView;

  /// The bottom document view
  DocumentWidget * documentWidget;

  QList<Document*> selectedDocuments();

public:

  DocumentsPage(Cabinet * cab);

  /// Returns the title of the page
  virtual QString pageTitle() override;
  virtual ~DocumentsPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static DocumentsPage * getDocumentsPage(Cabinet * cab);

public slots:

  /// Show the context menu...
  void treeViewContextMenu(const QPoint & pos);

protected slots:

  void onCurrentDocumentChanged(const QModelIndex &current,
                                const QModelIndex &previous);
};

#endif
