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

/// A small widget to edit a FilterElement
class FilterElementWidget : public QWidget {
  Q_OBJECT;

  /// The target FilterElement
  FilterElement * element;

  /// The attribute selection combo box
  QComboBox * attributeSelection;

  /// Text edition
  QLineEdit * edit;


  /// \name Buttons
  ///
  /// These buttons can be displayed or not using
  /// displayButtons(). They have no effect on the FilterElementWidget
  /// or its target FilterElement, but appropriate signals are emitted
  /// when needed.
  ///
  /// @{

  /// The + button to insert an element after this one.
  QPushButton * plusButton;

  /// The - button to delete this element.
  QPushButton * minusButton;
  /// @}
  
public:
  FilterElementWidget(FilterElement * el = NULL);

public slots:

  void targetChanged(int);

  void textChanged(const QString & str);
  
  void setFilterElement(FilterElement * el);

  /// To turn on/off the display of +/- buttons.
  void displayButtons(bool yes = true);

  /// Whether or not the widget is visible
  void tweakVisibility(bool visible);

signals:

  void plusPushed(FilterElementWidget * w);
  void minusPushed(FilterElementWidget * w);

private slots:
  
  void onMinusPushed();
  void onPlusPushed();
};

/// A more comprehensive widget that edits a whole QList of
/// FilterElement.
///
/// \todo This class should eventually hold a scrollable area
class FilterElementListWidget : public QWidget {
  Q_OBJECT;
  
  /// The target list of FilterElement
  QList<FilterElement> * target;

  /// The main layout of the "list"
  QVBoxLayout * mainLayout;

  /// The list of currently available FilterElementWidget
  QList<FilterElementWidget *> widgets;

  /// Returns the widget displaying element n
  FilterElementWidget * getNumberedWidget(int n);

  int findWidgetIndex(FilterElementWidget * w) const {
    return widgets.indexOf(w);
  };

public:

  FilterElementListWidget();

  /// Sets the target list; can be NULL. If the target list has no
  /// elements, a new one will be added.
  void setTarget(QList<FilterElement> * t);

public slots:
  
  /// Inserts an element after the given FilterElementWidget
  void insertElement(FilterElementWidget * w);

  /// Deletes the element corresponding to the given
  /// FilterElementWidget
  void deleteElement(FilterElementWidget * w);

};

#endif
