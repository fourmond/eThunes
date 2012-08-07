/**
    \file categorypage.hh
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

#ifndef __CATEGORYPAGE_HH
#define __CATEGORYPAGE_HH

#include <wallet.hh>
#include <navigationpage.hh>

class CategoryModel;

/// This NavigationPage displays various informations about
/// categories of a Wallet.
class CategoryPage : public NavigationPage {

  Q_OBJECT;

protected:

  /// A correspondance Wallet* -> CategoryPage
  static QHash<Wallet *, CategoryPage *> categoryPages;

  Wallet * wallet;

  /// Underlying model to display data about categories
  CategoryModel * model;

  /// And the corresponding view:
  QTreeView * view;


public:

  CategoryPage(Wallet * a);

  /// Returns the title of the page
  virtual QString pageTitle();

  virtual ~CategoryPage();

  /// Returns the AccountPage for the given account, or create it if
  /// it doesn't exist yet.
  static CategoryPage * getCategoryPage(Wallet * wallet);

public slots:

  /// Displays the context menu for tweaking
  void categoriesContextMenu(const QPoint & pos);

  /// Prompts the user for changing the color of the current category.
  void changeCurrentColor();

  /// Prompts the user for changing the color of the current category.
  void displayCurrentTransactions();
};

#endif
