/*
  evolvingitemwidget.cc: Implementation of NavigationWidget
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

#include <headers.hh>
#include <evolvingitemwidget.hh>

template <class T, class Widget> 
EvolvingItemWidget<T,Widget>::EvolvingItemWidget(EvolvingItem<T> * t, 
                                                 QWidget * parent) :
  QWidget(parent), target(t), isUpdatingWidget(false) {
}

template <class T, class Widget> 
void EvolvingItemWidget<T,Widget>::setupFrame()
{
  gridLayout = new QGridLayout(this);
  updateWidget();
}

template <class T, class Widget> 
void EvolvingItemWidget<T,Widget>::createLine(int i)
{
  QDateEdit * de = NULL;
  QLabel * l;
  if(i)
    de = new QDateEdit;
  dateEditors.append(de);
  if(i) {
    l = new QLabel(tr("Value after"));
    gridLayout->addWidget(l, i, 0);
    de->setCalendarPopup(true);
    gridLayout->addWidget(de, i, 1);
    l = new QLabel(tr(":"));
    gridLayout->addWidget(l, i, 2);
    connect(de, SIGNAL(dateChanged(const QDate&)),
            SLOT(somethingChanged()));
  }
  else {
    l = new QLabel(tr("First value:"));
    gridLayout->addWidget(l, i, 0, 1, 3);
  }


  Widget * w = createEditor(target->valueAt(i));
  valueEditors.append(w);
  gridLayout->addWidget(w, i, 3);

  QPushButton * bt = new QPushButton(tr("+"));
  gridLayout->addWidget(bt, i, 4);
  connect(bt, SIGNAL(clicked()), SLOT(addLine()));
  bt = new QPushButton(tr("-"));
  gridLayout->addWidget(bt, i, 5);
  if(! i)
    bt->setEnabled(false);

}

template <class T, class Widget> 
void EvolvingItemWidget<T,Widget>::realAddLine()
{
  target->itemChanges.append(target->pairValue(target->itemChanges.size()));
  updateWidget();
}

/// @bug when using the popup calendar, if one modifies the dates so
/// that the order changes, then swapped entries both become the same
/// value, since apparently the calendar sets the target value
/// twice. There are no other workarounds save not sorting.
template <class T, class Widget> 
void EvolvingItemWidget<T,Widget>::updateWidget()
{
  if(isUpdatingWidget)
    return;                     // Nothing to do

  isUpdatingWidget = true;

  for(int i = dateEditors.size(); 
      i <= target->itemChanges.size(); i++)
    createLine(i);

  for(int i = 0; i < dateEditors.size(); i++) {
    if(i <= target->itemChanges.size()) {
      if(i) {
        dateEditors[i]->setEnabled(true);
        dateEditors[i]->setDate(target->dateAt(i));
      }
      valueEditors[i]->setEnabled(true);
      setEditorValue(valueEditors[i], target->valueAt(i));
    }
    else {
      if(i)
        dateEditors[i]->setEnabled(false);
      valueEditors[i]->setEnabled(false);
    }
  }

  isUpdatingWidget = false;
}


template <class T, class Widget> 
void EvolvingItemWidget<T,Widget>::updateTarget()
{
  if(isUpdatingWidget)
    return;

  for(int i = 0; i <= target->itemChanges.size(); i++) {
    if(i) {
      target->dateAt(i) = dateEditors[i]->date();
    }
    target->valueAt(i) = getEditorValue(valueEditors[i]);
  }

  target->sort();

  updateWidget();
}


//////////////////////////////////////////////////////////////////////

EvolvingIntWidget::EvolvingIntWidget(EvolvingItem<int> * target, 
                                     QWidget * parent) :  
  EvolvingItemWidget<int, QSpinBox>(target, parent) {
  /// \b warning as any derived class will not use the correct virtual
  /// functions.
  setupFrame();
};


QSpinBox * EvolvingIntWidget::createEditor(const int & value)
{
  QSpinBox * editor = new QSpinBox();
  editor->setValue(value);
  editor->setRange(INT_MIN, INT_MAX);
  connect(editor, SIGNAL(valueChanged(int)),
          SLOT(somethingChanged()));
  return editor;
}

void EvolvingIntWidget::setEditorValue(QSpinBox * editor, const int & value)
{
  editor->setValue(value);
}

int EvolvingIntWidget::getEditorValue(QSpinBox * editor)
{
  return editor->value();
}

void EvolvingIntWidget::addLine()
{
  realAddLine();
}
void EvolvingIntWidget::somethingChanged()
{
  updateTarget();
}
