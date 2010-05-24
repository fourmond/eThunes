/**
    \file filterdialog.hh
    The dialog boxes for modifying filters of a Wallet.
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


#ifndef __FILTERDIALOG_HH
#define __FILTERDIALOG_HH

#include <filter.hh>
#include <wallet.hh>

class FilterDialog : public QDialog {
  Q_OBJECT;
  /// The wallet whose filters we'll be dealing with.
  Wallet * wallet;

  /// The list displaying the current filters.
  QListWidget * list;
public:
  FilterDialog(Wallet * w);

public slots:
  void newFilter();

  /// Updates the filter list according to what the wallet has.
  void updateFilterList();

  /// Edits the current filter
  void editCurrent();

  /// Run filters
  void runFilters();
};

/// A small widget to edit a FilterElement
class FilterElementWidget : public QWidget {
  Q_OBJECT;

  /// The target FilterElement
  FilterElement * element;

  /// The attribute selection combo box
  QComboBox * attributeSelection;
public:
  FilterElementWidget(FilterElement * el);

public slots:

  void targetChanged(int);

  void textChanged(const QString & str);

};

class FilterEditDialog : public QDialog {
  Q_OBJECT;

  /// The target wallet, for the record, and for category list ?
  Wallet * wallet;

  /// The target filter
  ///
  /// \todo work on a copy ?
  Filter * filter;
public:
  FilterEditDialog(Wallet *, Filter*);

public slots:
  void nameChanged(const QString & str);

  void categoryChanged(const QString & str);
};

#endif
