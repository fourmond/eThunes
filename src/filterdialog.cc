/*
    filterdialog.cc: dialog boxes for filter
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
#include <filterdialog.hh>

FilterDialog::FilterDialog(Wallet *w)
{
  wallet = w;
  // First, a VBox:
  QVBoxLayout * l1 = new QVBoxLayout(this);
  l1->addWidget(new QLabel(tr("Available filters")));

  QHBoxLayout * hb = new QHBoxLayout;
  l1->addLayout(hb);

  list = new QListWidget(this);
  hb->addWidget(list);
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
	  SLOT(editCurrent()));

  QVBoxLayout * l2 = new QVBoxLayout;
  QPushButton * bt = new QPushButton(tr("New filter"));
  connect(bt, SIGNAL(clicked()), SLOT(newFilter()));
  l2->addWidget(bt);
  bt = new QPushButton(tr("Edit filter"));
  connect(bt, SIGNAL(clicked()), SLOT(editCurrent()));
  l2->addWidget(bt);
  l2->addSpacing(10);
  bt = new QPushButton(tr("Run filters"));
  connect(bt, SIGNAL(clicked()), SLOT(runFilters()));
  l2->addWidget(bt);
  l2->addSpacing(10);
  bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l2->addWidget(bt);
  hb->addLayout(l2);

  updateFilterList();
}

void FilterDialog::updateFilterList()
{
  list->clear();
  for(int i = 0; i < wallet->filters.size(); i++)
    list->addItem(wallet->filters[i].name);
}

void FilterDialog::newFilter()
{
  wallet->filters.push_back(Filter());
  Filter * f = &wallet->filters.last();
  f->name = tr("New filter");
  updateFilterList();
  list->setCurrentRow(list->count()-1);
  // And edit
  editCurrent();
}


void FilterDialog::runFilters()
{
  wallet->runFilters();
}

void FilterDialog::editCurrent()
{
  FilterEditDialog * edit =
    new FilterEditDialog(wallet, &wallet->filters[list->currentRow()]);
  connect(edit, SIGNAL(finished(int)), SLOT(updateFilterList()));
  edit->show();
}


FilterEditDialog::FilterEditDialog(Wallet *w, Filter * f)
{
  wallet = w;
  filter = f;

  QVBoxLayout * l1 = new QVBoxLayout(this);
  QHBoxLayout * hb = new QHBoxLayout;
  hb->addWidget(new QLabel(tr("Filter: ")));
  QLineEdit * edit = new QLineEdit(filter->name);
  connect(edit, SIGNAL(textChanged(const QString &)),
	  SLOT(nameChanged(const QString &)));
  hb->addWidget(edit);
  l1->addLayout(hb);

  if(! f->elements.size())
    f->elements.push_back(FilterElement());
  for(int i = 0; i < f->elements.size(); i++)
    l1->addWidget(new FilterElementWidget(&f->elements[i]));

  hb = new QHBoxLayout;
  hb->addWidget(new QLabel(tr("Target category: ")));
  edit = new QLineEdit(filter->category);
  connect(edit, SIGNAL(textChanged(const QString &)),
	  SLOT(categoryChanged(const QString &)));
  hb->addWidget(edit);
  l1->addLayout(hb);


  QPushButton * bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(bt);
}

void FilterEditDialog::nameChanged(const QString & str)
{
  filter->name = str;
}

void FilterEditDialog::categoryChanged(const QString & str)
{
  filter->category = str;
}
