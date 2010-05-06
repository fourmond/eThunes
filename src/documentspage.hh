/** 
    \file documentspage.hh
    A page displaying informations about the categories
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

#ifndef __DOCUMENTSPAGE_HH
#define __DOCUMENTSPAGE_HH

#include <wallet.hh>
#include <navigationpage.hh>
#include <documentsmodel.hh>

#include <cabinet.hh>

/// This NavigationPage displays all the documents contained in a
/// given Cabinet.
///
/// \todo Add an icon for the file, links working for collections,
/// maybe a raw display of the attributes using an appropriate dialog
/// box ?
class DocumentsPage : public NavigationPage {

  Q_OBJECT;

protected:
  /// A correspondance Wallet* -> DocumentsPage
  static QHash<Cabinet *, DocumentsPage *> documentsPages;


  /// Underlying model to display data about categories
  DocumentsModel * model;

  /// The current list of documents to be displayed.
  QList<Document *> documents;

  /// And the corresponding view:
  QTreeView * view;

  /// The Cabinet we're taking care of.
  Cabinet * cabinet;

  
public:
  
  DocumentsPage(Cabinet * c);
  
  /// Returns the title of the page
  virtual QString pageTitle();
  
  virtual ~DocumentsPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static DocumentsPage * getDocumentsPage(Cabinet * cabinet);

public slots:

  /// Updates the page. Mostly useless for now
  void updateContents();

  // /// Displays the context menu for tweaking
  // void documentsContextMenu(const QPoint & pos);

};

#endif
