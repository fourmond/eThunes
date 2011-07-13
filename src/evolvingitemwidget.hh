/**
   \file evolvingitemwidget.hh
   Widget editing EvolvingItem
   Copyright 2011 by Vincent Fourmond

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

#ifndef __EVOLVINGITEMWIDGET_HH
#define __EVOLVINGITEMWIDGET_HH

#include <evolvingitem.hh>

/// A template base class for editing EvolvingItem objects.
///
/// As a special case, template code is not in the header file, but in
/// the code file, as derived classes will end up being there too.
template <class T, class Widget> 
class EvolvingItemWidget : public QWidget {
protected:
  EvolvingItem<T> * target;

  /// The main grid layout
  QGridLayout * gridLayout;

  /// The date editors. 
  QList<QDateEdit * > dateEditors;

  /// The value editors.
  QList<Widget * > valueEditors;

  QButtonGroup * deleteButtonGroup();

  /// Creates an editor
  virtual Widget * createEditor(const T & value) = 0;

  /// Sets the value to the given editor
  virtual void setEditorValue(Widget * editor, const T & value) = 0;

  /// Gets the value from the editor.
  virtual T getEditorValue(Widget * editor) = 0;

  /// create all editors for the given line.
  void createLine(int i);
  
  /// Reads data from the target to the editors
  void updateWidget();

  /// Reads the data from the editors to the targets
  void updateTarget();

  /// Avoid infinite loops between both update functions
  bool isUpdatingWidget;


  /// This function sets the frame up. It \b must be called in the
  /// constructor of the \b derived class.
  void setupFrame(); 

  /// To be called from the addLine derived slot
  void realAddLine();
public:

  EvolvingItemWidget(EvolvingItem<T> * target, QWidget * parent = NULL);

public /*slots*/:                   // Not real slots, just for memory

  virtual void addLine() = 0;          // must call realAddLine
  virtual void somethingChanged() = 0; // must call updateTarget

};

/// Specialization of EvolvingItemWidget for editing ints.
class EvolvingIntWidget : public EvolvingItemWidget<int, QSpinBox> {
  Q_OBJECT;
protected:
  virtual QSpinBox * createEditor(const int & value);
  virtual void setEditorValue(QSpinBox * editor, const int & value);
  virtual int getEditorValue(QSpinBox * editor);

public:
  EvolvingIntWidget(EvolvingItem<int> * target, QWidget * parent = NULL);

public slots:
  void addLine();
  void somethingChanged();

};

#endif
