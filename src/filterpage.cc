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
#include <filterpage.hh>
#include <filterdialog.hh>	// For FilterElementWidget 

QHash<Wallet *, FilterPage *> FilterPage::filterPages;

FilterPage * FilterPage::getFilterPage(Wallet * w)
{
  if(!filterPages.contains(w))
    filterPages[w] = new FilterPage(w);
  return filterPages[w];
}

FilterPage::FilterPage(Wallet *w) : backupFilter(0)
{
  wallet = w;
  QVBoxLayout * l1 = new QVBoxLayout(this);
  l1->addWidget(new QLabel(tr("Filters for the account")));

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
  bt = new QPushButton(tr("Edit as new"));
  connect(bt, SIGNAL(clicked()), SLOT(editCurrent()));
  l2->addWidget(bt);
  l2->addSpacing(10);
  bt = new QPushButton(tr("Run all filters"));
  connect(bt, SIGNAL(clicked()), SLOT(runFilters()));
  l2->addWidget(bt);
  l2->addSpacing(10);
  bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l2->addWidget(bt);
  hb->addLayout(l2);

  // Now, we start another

  hb = new QHBoxLayout;
  hb->addWidget(new QLabel(tr("Current filter: ")));
  filterNameEdit = new QLineEdit(/*filter->name*/);
  connect(filterNameEdit, SIGNAL(textChanged(const QString &)),
	  SLOT(filterNameChanged(const QString &)));
  hb->addWidget(filterNameEdit);
  l1->addLayout(hb);

  // if(! f->elements.size())
  //   f->elements.push_back(FilterElement());
  // for(int i = 0; i < f->elements.size(); i++)
  //   l1->addWidget(new FilterElementWidget(&f->elements[i]));

  hb = new QHBoxLayout;
  hb->addWidget(new QLabel(tr("Target category: ")));
  // Here, use combo box !
  targetCategoryCombo = new CategoryCombo(wallet);
  // connect(edit, SIGNAL(textChanged(const QString &)),
  // 	  SLOT(categoryChanged(const QString &)));
  hb->addWidget(targetCategoryCombo);
  l1->addLayout(hb);


  hb = new QHBoxLayout;
  bt = new QPushButton(tr("Undo changes"));
  connect(bt, SIGNAL(clicked()), SLOT(undoFilterChanges()));
  hb->addWidget(bt);
  bt = new QPushButton(tr("Test filter"));
  hb->addWidget(bt);
  bt = new QPushButton(tr("Run filter"));
  hb->addWidget(bt);
  // connect(bt, SIGNAL(clicked()), SLOT(undoFilterChanges()));
  hb->addWidget(bt);
  l1->addLayout(hb);

  updateFilterList();
}

void FilterPage::updateFilterList()
{
  list->clear();
  for(int i = 0; i < wallet->filters.size(); i++)
    list->addItem(wallet->filters[i].name);
}

void FilterPage::newFilter()
{
  wallet->filters.push_back(Filter());
  Filter * f = &wallet->filters.last();
  f->name = tr("New filter");
  updateFilterList();
  list->setCurrentRow(list->count()-1);
  // And edit
  editCurrent();
}


void FilterPage::runFilters()
{
  wallet->runFilters();
}

void FilterPage::editCurrent()
{
  // FilterEditPage * edit =
  //   new FilterEditPage(wallet, &wallet->filters[list->currentRow()]);
  // connect(edit, SIGNAL(finished(int)), SLOT(updateFilterList()));
  // edit->show();
}

void FilterPage::undoFilterChanges()
{
}


// FilterEditPage::FilterEditPage(Wallet *w, Filter * f)
// {
//   wallet = w;
//   filter = f;

// }

// void FilterEditPage::nameChanged(const QString & str)
// {
//   filter->name = str;
// }

// void FilterEditPage::categoryChanged(const QString & str)
// {
//   filter->category = str;
// }
