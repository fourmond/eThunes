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
#include <categorycombo.hh>

class FilterPage : public NavigationPage {
  Q_OBJECT;
  /// The wallet whose filters we'll be dealing with.
  Wallet * wallet;

  /// The list displaying the current filters.
  QListWidget * list;

  /// This takes the contents of the currently selected filter before
  /// it is modified, so as to act as a backup
  Filter * backupFilter; 

  /// The name of the Filter
  QLineEdit * filterNameEdit;

  /// The combo box to edit target category
  CategoryCombo * targetCategoryCombo;

  /// A correspondance Wallet -> FilterPage, though in principle only
  /// one Wallet should ever be used ;-)...
  static QHash<Wallet *, FilterPage *> filterPages;

  /// Kept up-to-date by filterChanged();
  Filter * currentFilter;

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

  /// Run filters
  void runFilters();

  /// Copies back the contents of the
  void undoFilterChanges();

  /// Called when the currently selected filter has changed: simply
  /// updates
  void filterChanged();

  /// Called when the name has changed.
  void filterNameChanged(const QString & text);

  /// Called when the category
  void filterCategoryChanged(const QString & text);

protected:
  /// Fills in the information of the filter to the target 
  void fillListItemWithFilter(QListWidgetItem * item, Filter * filter) const ;

  /// Returns a newly created QListWidgetItem for the given filter
  QListWidgetItem * filterItem(Filter * filter) const {
    QListWidgetItem * item = new QListWidgetItem;
    fillListItemWithFilter(item, filter);
    return item;
  };

  /// Updates the currently selected item (ie, copies data back from
  /// the filter to the list).
  void updateCurrentListItem(); 
};

#endif
