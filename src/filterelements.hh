/**
   \file filterelements.hh
   Widgets for editing a list of filter elements
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


#ifndef __FILTERELEMENTS_HH
#define __FILTERELEMENTS_HH

#include <filter.hh>
#include <wallet.hh>

/// A small widget to edit a FilterElement
class FilterElementWidget : public QWidget {
  Q_OBJECT;

  /// The target FilterElement
  FilterElement * element;

  /// The attribute selection combo box
  QComboBox * attributeSelection;

  /// Text edition
  QLineEdit * edit;
  
public:
  FilterElementWidget(FilterElement * el = NULL);

public slots:

  void targetChanged(int);

  void textChanged(const QString & str);
  
  void setFilterElement(FilterElement * el);

};

/// A more comprehensive widget that edits a whole QList of
/// FilterElement.
class FilterElementListWidget : public QWidget {
  
};

#endif
