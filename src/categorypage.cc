/*
    categorypage.cc: Implementation of CategoryPage
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
#include <categorypage.hh>
#include <transactionlistdialog.hh>

CategoryPage::CategoryPage(Wallet * w) : wallet(w)
{

  QVBoxLayout * layout = new QVBoxLayout(this);
  model = new CategoryModel(wallet);
  view = new QTreeView();
  layout->addWidget(view);
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  // view->setRootIsDecorated(false);
  view->setContextMenuPolicy(Qt::CustomContextMenu);
  view->setAlternatingRowColors(true);
  updateContents();
  connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(categoriesContextMenu(const QPoint &)));
}


CategoryPage::~CategoryPage()
{
}

QString CategoryPage::pageTitle()
{
  return tr("Categories");
}

QHash<Wallet *, CategoryPage *> CategoryPage::categoryPages;

CategoryPage * CategoryPage::getCategoryPage(Wallet * wallet)
{
  if(! categoryPages.contains(wallet))
    categoryPages[wallet] = new CategoryPage(wallet);
  return categoryPages[wallet];
}

void CategoryPage::updateContents()
{
  // // For now, we only display top-leve categories
  // QString text = "<table><tr><th><strong>" + tr("Category") + "</strong></th>"
  //   "<th><strong>" + tr("Total") + "</strong></th></tr>";
  // QStringList toplevelCategories = wallet->categories.keys();
  // toplevelCategories.sort();
  // for(int i = 0; i < toplevelCategories.count(); i++) {
  //   Category * c = &(wallet->categories[toplevelCategories[i]]);
  //   int amount = 0;
  //   QList<Transaction *> transactions =
  //     wallet->categoryTransactions(c);
  //   for(int j = 0; j < transactions.size(); j++)
  //     amount += transactions[j]->amount;
  //   text += "<tr><td>" + toplevelCategories[i] + "</td><td>" +
  //     Transaction::formatAmount(amount) + "</td></tr>";
  // }
  // text += "</table>";
  // contents->setText(text);
}

void CategoryPage::categoriesContextMenu(const QPoint & pos)
{
  QModelIndex i = view->indexAt(pos);
  Category * c = model->indexedCategory(i);
  if(c) {
    // We popup a menu for modifying a few things.
    QMenu * menu = new QMenu();
    menu->addAction("Change category color",this,
		    SLOT(changeCurrentColor()));
    menu->addAction("Display category transactions",this,
		    SLOT(displayCurrentTransactions()));
    menu->exec(view->viewport()->mapToGlobal(pos));
  }
}

void CategoryPage::changeCurrentColor()
{
  QModelIndex i = view->currentIndex();
  Category * c = model->indexedCategory(i);
  if(c) {
    QColorDialog dlg(c->categoryColor());
    dlg.setWindowTitle(tr("Pick the color for %1").arg(c->fullName()));
    if(dlg.exec() == QDialog::Accepted) {
      c->color = dlg.currentColor();
    }
  }
}

void CategoryPage::displayCurrentTransactions()
{
  QModelIndex i = view->currentIndex();
  Category * c = model->indexedCategory(i);
  if(c) {
    TransactionListDialog * ct = new TransactionListDialog();
    ct->displayCategory(c, wallet);
    ct->show();
  }
}
