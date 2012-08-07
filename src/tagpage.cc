/*
    tagpage.cc: Implementation of TagPage
    Copyright 2010, 2012 by Vincent Fourmond

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
#include <tagpage.hh>
#include <statisticsmodel.hh>
#include <transactionlistdialog.hh>

TagPage::TagPage(Wallet * w) : wallet(w)
{

  QVBoxLayout * layout = new QVBoxLayout(this);
  model = new TagModel(wallet);
  view = new QTreeView();
  layout->addWidget(view);
  view->setModel(model);
  view->setRootIndex(model->index(0,0));
  // view->setRootIsDecorated(false);
  view->setContextMenuPolicy(Qt::CustomContextMenu);
  view->setAlternatingRowColors(true);
  updateContents();
  connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
	  SLOT(tagsContextMenu(const QPoint &)));
}


TagPage::~TagPage()
{
}

QString TagPage::pageTitle()
{
  return tr("Tags");
}

QHash<Wallet *, TagPage *> TagPage::tagPages;

TagPage * TagPage::getTagPage(Wallet * wallet)
{
  if(! tagPages.contains(wallet))
    tagPages[wallet] = new TagPage(wallet);
  return tagPages[wallet];
}

void TagPage::updateContents()
{
  // // For now, we only display top-leve categories
  // QString text = "<table><tr><th><strong>" + tr("Tag") + "</strong></th>"
  //   "<th><strong>" + tr("Total") + "</strong></th></tr>";
  // QStringList toplevelCategories = wallet->categories.keys();
  // toplevelCategories.sort();
  // for(int i = 0; i < toplevelCategories.count(); i++) {
  //   Tag * c = &(wallet->categories[toplevelCategories[i]]);
  //   int amount = 0;
  //   QList<Transaction *> transactions =
  //     wallet->tagTransactions(c);
  //   for(int j = 0; j < transactions.size(); j++)
  //     amount += transactions[j]->amount;
  //   text += "<tr><td>" + toplevelCategories[i] + "</td><td>" +
  //     Transaction::formatAmount(amount) + "</td></tr>";
  // }
  // text += "</table>";
  // contents->setText(text);
}

void TagPage::tagsContextMenu(const QPoint & pos)
{
  QModelIndex i = view->indexAt(pos);
  Tag * c = model->indexedTag(i);
  if(c) {
    // We popup a menu for modifying a few things.
    QMenu * menu = new QMenu();
    menu->addAction("Display tag transactions",this,
        	    SLOT(displayCurrentTransactions()));
    menu->exec(view->viewport()->mapToGlobal(pos));
  }
}

void TagPage::changeCurrentColor()
{
  // QModelIndex i = view->currentIndex();
  // Tag * c = model->indexedTag(i);
  // if(c) {
  //   QColorDialog dlg(c->tagColor());
  //   dlg.setWindowTitle(tr("Pick the color for %1").arg(c->fullName()));
  //   if(dlg.exec() == QDialog::Accepted) {
  //     c->color = dlg.currentColor();
  //   }
  // }
}

void TagPage::displayCurrentTransactions()
{
  QModelIndex i = view->currentIndex();
  Tag * c = model->indexedTag(i);
  if(c) {
    TransactionListDialog * ct = new TransactionListDialog();
    ct->displayTag(c, wallet);
    ct->show();
  }
}
