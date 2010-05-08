/** 
    \file collectionpage.hh
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

#ifndef __COLLECTIONPAGE_HH
#define __COLLECTIONPAGE_HH

#include <collection.hh>
#include <navigationpage.hh>

#include <walletdw.hh>
#include <collectionsdw.hh>

/// This NavigationPage displays a Collection.
///
/// \todo It should be possible to reparse all the documents of a
/// Collection (or at least of a given type) to take part of new
/// features developed
class CollectionPage : public NavigationPage {

  Q_OBJECT;

protected:


  /// The displayed Collection
  Collection * collection;


  /// The actual contents of the display.
  ///
  /// \todo This shouldn't be a label, since it can lead to spurious
  /// resizing of the parent widget. Few solutions:
  /// \li use a readonly QTextEdit
  /// \li wrap a QLabel into a QScrollArea
  /// \li write yet another custom widget for displaying text
  ///
  /// In any case, this won't do on the long term.
  QLabel * summary;

  /// A correspondance Collection* -> CollectionPage
  static QHash<Collection *, CollectionPage *> collectionPages;

public:

  CollectionPage(Collection * c);
  
  /// Returns the title of the page
  virtual QString pageTitle();
  
  virtual ~CollectionPage();

  /// Returns the CollectionPage for the given collection, or create
  /// it if it doesn't exist yet.
  static CollectionPage * getCollectionPage(Collection * collection);

signals:

public slots:

  /// Updates the page
  void updateContents();

  /// Follows an URL
  void openURL(const QString & url);

  /// Prompt for a file attachment for the given Document
  void promptForFileAttachment(Document * doc);

};

#endif
