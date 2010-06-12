/*
  filterelements.cc: edition of filter elements
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

#include <headers.hh>
#include <filterelements.hh>

FilterElementWidget::FilterElementWidget(FilterElement * el) : element(NULL)
{
  QHBoxLayout * hb = new QHBoxLayout(this);
  hb->addWidget(new QLabel(tr("Applies to:")));
  attributeSelection = new QComboBox;
  attributeSelection->setEditable(false);

  attributeSelection->addItem(tr("Name"), QVariant(FilterElement::Name));
  attributeSelection->addItem(tr("Memo"), QVariant(FilterElement::Memo));
  connect(attributeSelection, SIGNAL(activated(int)),
	  SLOT(targetChanged(int)));

  hb->addWidget(attributeSelection);

  edit = new QLineEdit();
  connect(edit, SIGNAL(textChanged(const QString &)),
	  SLOT(textChanged(const QString &)));

  hb->addWidget(edit);

  plusButton = new QPushButton("+");
  hb->addWidget(plusButton);
  connect(plusButton, SIGNAL(clicked(bool)),
	  SLOT(onPlusPushed()));

  minusButton = new QPushButton("-");
  hb->addWidget(minusButton);
  connect(minusButton, SIGNAL(clicked(bool)),
	  SLOT(onMinusPushed()));

  /// \todo Regexp ?
  setFilterElement(el);
}

void FilterElementWidget::onMinusPushed()
{
  emit(minusPushed(this));
}

void FilterElementWidget::onPlusPushed()
{
  emit(plusPushed(this));
}

void FilterElementWidget::displayButtons(bool yes)
{
  minusButton->setVisible(yes);
  plusButton->setVisible(yes);
}

void FilterElementWidget::setFilterElement(FilterElement * el)
{
  element = el;
  if(! element) {
    tweakVisibility(false); 		// Or could be hidden ?
    edit->setText("");
    return;
  }

  tweakVisibility(true);
  edit->setText(element->match);
  if(element->transactionAttribute == FilterElement::Name)
    attributeSelection->setCurrentIndex(attributeSelection->count()-2);
  else if(element->transactionAttribute == FilterElement::Memo)
    attributeSelection->setCurrentIndex(attributeSelection->count()-1);
}

void FilterElementWidget::tweakVisibility(bool visible)
{
  // in here, we hide everything but a small label at the beginning of
  // the line
  setEnabled(visible);
  // attributeSelection->setVisible(visible);
}

void FilterElementWidget::textChanged(const QString & str)
{
  if(element)
    element->match = str;
}

void FilterElementWidget::targetChanged(int t)
{
  // Ugly, but, well, works...
  if(element)
    *((int*)&element->transactionAttribute) =
      attributeSelection->itemData(t).toInt();
}

//////////////////////////////////////////////////////////////////////

FilterElementListWidget::FilterElementListWidget() :
  target(NULL)
{
  mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(2); // small by default

  getNumberedWidget(3); // We only ensure there is one element visible
}

FilterElementWidget * FilterElementListWidget::getNumberedWidget(int i)
{
  while(widgets.size() <= i) {
    FilterElementWidget * w = new FilterElementWidget;
    mainLayout->addWidget(w);
    widgets.append(w);
    connect(w, SIGNAL(plusPushed(FilterElementWidget *)),
	    SLOT(insertElement(FilterElementWidget *)));
    connect(w, SIGNAL(minusPushed(FilterElementWidget *)),
	    SLOT(deleteElement(FilterElementWidget *)));
    w->show();
  }
  return widgets[i];
}

void FilterElementListWidget::setTarget(QList<FilterElement> * t)
{
  target = t;
  if(! target) {
    for(int i = 0; i < widgets.size(); i++)
      widgets[i]->setFilterElement(NULL);
    return;
  }
  if(target->size() == 0)
    target->append(FilterElement());
  int max = widgets.size();
  if(max < target->size())
    max = target->size();
  for(int i = 0; i < max; i++)
    getNumberedWidget(i)->
      setFilterElement(target->size() > i ? &target->operator[](i) : NULL);
}

void FilterElementListWidget::insertElement(FilterElementWidget * w)
{
  int idx = findWidgetIndex(w);
  if(idx < 0 || !target)
    return; 			// But, really, this shouldn't happen
  target->insert(idx + 1, FilterElement()); 
  setTarget(target); // not satisfying, but not too bad, in real
}

void FilterElementListWidget::deleteElement(FilterElementWidget * w)
{
  int idx = findWidgetIndex(w);
  if(idx < 0 || !target)
    return; 			// But, really, this shouldn't happen
  target->removeAt(idx); 
  setTarget(target); // not satisfying, but not too bad, in real
}

