/**
    \file tagpage.hh
    A page displaying informations about the tags
    Copyright 2010, 2012 by Vincent Fourmond

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

#ifndef __TAGPAGE_HH
#define __TAGPAGE_HH

#include <wallet.hh>
#include <navigationpage.hh>

class TagModel;

/// This NavigationPage displays various informations about
/// categories of a Wallet.
///
/// @todo Share some code with CategoryPage ?
class TagPage : public NavigationPage {

  Q_OBJECT;

protected:

  /// A correspondance Wallet* -> TagPage
  static QHash<Wallet *, TagPage *> tagPages;

  Wallet * wallet;

  /// Underlying model to display data about categories
  TagModel * model;

  /// And the corresponding view:
  QTreeView * view;


public:

  TagPage(Wallet * a);

  /// Returns the title of the page
  virtual QString pageTitle();

  virtual ~TagPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static TagPage * getTagPage(Wallet * wallet);

public slots:

  /// Updates the page. Mostly useless for now
  void updateContents();

  /// Displays the context menu for tweaking
  void categoriesContextMenu(const QPoint & pos);

  /// Prompts the user for changing the color of the current tag.
  void changeCurrentColor();

  /// Prompts the user for changing the color of the current tag.
  void displayCurrentTransactions();
};

#endif
