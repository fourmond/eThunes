/**
    \file documentpage.hh
    A page displaying the contents of a Document.
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

#ifndef __DOCUMENTPAGE_HH
#define __DOCUMENTPAGE_HH

#include <wallet.hh>
#include <navigationpage.hh>
#include <document.hh>

/// This NavigationPage displays various informations about
/// categories of a Wallet.
class DocumentPage : public NavigationPage {

  Q_OBJECT;

protected:

  /// A correspondance Document* -> DocumentPage
  static QHash<Document *, DocumentPage *> documentPages;

  Document * document;

public:

  DocumentPage(Document * a);

  /// Returns the title of the page
  virtual QString pageTitle();

  virtual ~DocumentPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static DocumentPage * getDocumentPage(Document * document);

public slots:

  /// Updates the page. Mostly useless for now
  void updateContents();

};

#endif
