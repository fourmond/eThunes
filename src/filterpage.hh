/**
    \file filterpage.hh
    The page boxes for modifying filters of a Wallet.
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


#ifndef __FILTERPAGE_HH
#define __FILTERPAGE_HH

#include <filter.hh>
#include <navigationpage.hh>
#include <wallet.hh>

class FilterPage : public NavigationPage {
  Q_OBJECT;
  /// The wallet whose filters we'll be dealing with.
  Wallet * wallet;

  /// The list displaying the current filters.
  QListWidget * list;

  Filter * currentlySelectedFilter() const;

  /// The name of the Filter
  QLineEdit * filterNameEdit;

  /// A correspondance Wallet -> FilterPage, though in principle only
  /// one Wallet should ever be used ;-)...
  static QHash<Wallet *, FilterPage *> filterPages;

public:
  FilterPage(Wallet * w);

  /// \todo This function should be const one day.
  virtual QString pageTitle() {
    return tr("Filters");
  };

  static FilterPage * getFilterPage(Wallet * w);

public slots:
  void newFilter();

  /// Updates the filter list according to what the wallet has.
  void updateFilterList();

  /// Edits the current filter
  void editCurrent();

  /// Run filters
  void runFilters();

  void undoFilterChanges();

protected slots:
  
  // /// Display the given filter down
  // void displayFilter();
};

#endif
