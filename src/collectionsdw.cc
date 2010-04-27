/*
    collectionsdw.cc: Implementation of WalletDW
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
#include <collectionsdw.hh>

// #include <navigationwidget.hh>

CollectionsDW::CollectionsDW(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);
  connect(summary, SIGNAL(linkActivated(const QString &)), 
	  SLOT(showURL(const QString &)));

  // connect(, SIGNAL(accountsChanged()),
  // 	  SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

void CollectionsDW::updateSummary()
{
  QString text = QString("<strong>") + tr("Collections") + "</strong>\n<p>";
  QString cellStyle = " style='padding-right: 20px'";

  /// \todo Maybe the facility for building up tables should end up
  /// somewhere as global utilities ?
  text += "<table>\n";
  text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
    arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < cabinet->collections.size(); i++) {
    Collection * c = &cabinet->collections[i];
    text += QString("<tr><td" + cellStyle +"><a href='collection:%1'>").
      arg(i) + QString("%1</a></td><td align='right'>%2</td></tr>\n").
      arg(c->name).arg(c->documents.size());
  }
  text += "<tr></tr>";
  text += "</table>\n";
  
  summary->setText(text);
}

CollectionsDW::~CollectionsDW()
{
  
}

void CollectionsDW::showURL(const QString & link)
{
  // QStringList l = link.split(':');
  // QString & id = l[0];
  // NavigationPage * page = NULL;
  // if(id == "account") {
  //   page = AccountPage::getAccountPage(&wallet->accounts[l[1].toInt()]);
  // }
  // else if(id == "categories") {
  //   page = CategoryPage::getCategoryPage(wallet);
  // }
  // else if(id == "filters") {
  //   manageFilters(); /// \todo shouldn't this be a page too ?
  // }
  // if(page)
  //   NavigationWidget::gotoPage(page);
}

