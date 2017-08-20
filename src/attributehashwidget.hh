/**
    \file attributehashwidget.hh
    A widget for editing a Attributehash
    Copyright 2017 by Vincent Fourmond

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


#ifndef __ATTRIBUTEHASHWIDGET_HH
#define __ATTRIBUTEHASHWIDGET_HH

#include <attributehash.hh>

/// A helper class displaying just a single element in a attribute
/// hash, with the given possibilities:
/// @li choice of element name
/// @li choice of element type
/// @li choice of element value
/// @li buttons for adding/deleting (forwarded to the parent)
/// @li the possibility to lock name/type
///
/// @todo This class should probably be of limited visibility.
class AttributeHashElementWidget : public QWidget {
  Q_OBJECT;

  
  
  /// The editor for the line edit
  QLineEdit * nameEditor;

  /// The last named
  QString lastName;

  /// The combobox for the type
  QComboBox * typeEditor;

  /// The widget for the edition
  QWidget * valueEditor;

  /// The button for deletion
  QPushButton * delButton;

  /// The button for addition
  QPushButton * addButton;

  /// The layout
  QHBoxLayout * layout;

  /// The target hash
  AttributeHash * target;

  AttributeHash::HandledType currentType;
  
  void setType(AttributeHash::HandledType type);

public:

  AttributeHashElementWidget(AttributeHash * target, const QString & element,
                             AttributeHash::HandledType initialType,
                             QWidget * parent = NULL);

  /// If locked, the user cannot delete, change name or type
  void setLocked(bool locked = true);

signals:
  void deletePressed();
  void addPressed();

protected slots:
  void onValueChanged();
  void onNameChanged(const QString & ne);
  void onTypeChanged(const QString & nt);
  
};

/// A widget displaying the contents of a attributehash, possibly offering
/// edition of the attributehash type
class AttributeHashWidget : public QWidget {
  Q_OBJECT;

  /// AttributeHash being displayed.
  AttributeHash * attributehash;

  /// Sets up the frame
  void setupFrame();

  /// These are the attributes that are required, and have a fixed type.
  QHash<QString, AttributeHash::HandledType> fixedAttributes;

public:

  AttributeHashWidget(QWidget * parent = NULL);
};


#endif
