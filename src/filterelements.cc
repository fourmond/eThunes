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

FilterElementWidget::FilterElementWidget(FilterElement * el)
{
  element = el;
  QHBoxLayout * hb = new QHBoxLayout(this);
  hb->addWidget(new QLabel(tr("Applies to:")));
  attributeSelection = new QComboBox;
  attributeSelection->setEditable(false);
  attributeSelection->addItem(tr("Name"), QVariant(FilterElement::Name));
  if(element->transactionAttribute == FilterElement::Name)
    attributeSelection->setCurrentIndex(attributeSelection->count()-1);

  attributeSelection->addItem(tr("Memo"), QVariant(FilterElement::Memo));
  if(element->transactionAttribute == FilterElement::Memo)
    attributeSelection->setCurrentIndex(attributeSelection->count()-1);
  connect(attributeSelection, SIGNAL(activated(int)),
	  SLOT(targetChanged(int)));

  hb->addWidget(attributeSelection);

  QLineEdit * edit = new QLineEdit();
  edit->setText(element->match);
  connect(edit, SIGNAL(textChanged(const QString &)),
	  SLOT(textChanged(const QString &)));

  hb->addWidget(edit);


  /// \todo Regexp ?

}

void FilterElementWidget::textChanged(const QString & str)
{
  element->match = str;
}

void FilterElementWidget::targetChanged(int t)
{
  // Ugly, but, well, works...
  *((int*)&element->transactionAttribute) =
    attributeSelection->itemData(t).toInt();
}
