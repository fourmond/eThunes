/*
    walletdw.cc: Implementation of WalletDW
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
#include <walletdw.hh>

#include <filterdialog.hh>
#include <navigationwidget.hh>
#include <filterpage.hh>
#include <categorypage.hh>

#include <htlabel.hh>
#include <httarget-templates.hh>


WalletDW::WalletDW(Wallet * w) : wallet(w)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);
  // LinksHandler::handleObject(summary);

  connect(*wallet, SIGNAL(changed(const Watchdog *)),
	  SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

void WalletDW::showFiltersPage()
{
  NavigationWidget::gotoPage(FilterPage::getFilterPage(wallet));
}

void WalletDW::showCategoriesPage()
{
  NavigationWidget::gotoPage(CategoryPage::getCategoryPage(wallet));
}

void WalletDW::updateSummary()
{
  QString text = tr("<h2>Wallet</h2>");
  QString cellStyle = " style='padding-right: 20px'";
  int totalBalance = 0; /// \todo Maybe this should go in Wallet ?

  text += HTTarget::linkToMember(tr("%1 categories").
                                 arg(wallet->categories.categoryCount()),
                                 this, &WalletDW::showCategoriesPage)
    + "\n" + HTTarget::linkToMember(tr("%1 filters").
                                    arg(wallet->filters.count()),
                                    this, &WalletDW::showFiltersPage)
    + "<p>\n";

  /// \todo Maybe the facility for building up tables should end up
  /// somewhere as global utilities ?
  text += "<h3>Accounts</h3><table>\n";
  text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
    arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < wallet->accounts.size(); i++) {
    Account * ac = &wallet->accounts[i];
    text += QString("<tr><td" + cellStyle +">") +
      HTTarget::linkTo(ac->name(), ac) +
      QString("</td><td align='right'>%1</td></tr>\n").
      arg(Transaction::formatAmount(ac->balance()));
    totalBalance += ac->balance();
  }
  text += "<tr></tr>";
  text += (QString("<tr><td><strong>") + tr("Total") +
	   "</strong></td><td><strong>%1</strong></td></tr>\n").
    arg(Transaction::formatAmount(totalBalance));
  text += "</table>\n";


  if(wallet->accountGroups.size() > 0) {
    text += "<h3>Groups</h3>\n<table>\n";
    for(int i = 0; i < wallet->accountGroups.size(); i++) {
      const AccountGroup & g = wallet->accountGroups[i];
      text += QString("<tr><td" + cellStyle +">") +
        g.name + 
        QString("</td><td align='right'>%1</td></tr>\n").
        arg(Transaction::formatAmount(g.balance()));
    }
    text += "</table>\n";
  } 
  else {
    text += "<br>(no groups)<br>";
  }

  summary->setText(text);
}

WalletDW::~WalletDW()
{

}

void WalletDW::fileImportDialog()
{
  QStringList files =
    QFileDialog::getOpenFileNames(this,
				  tr("Select files to import"),
				  QString(),
				  tr("OFX files (*.ofx)"));
  if(! files.size())
    return;
  for(int i = 0; i < files.size(); i++) {
    OFXImport import = OFXImport::importFromFile(files[i]);
    wallet->importAccountData(import);
  }
  updateSummary();
}

void WalletDW::manageFilters()
{
  FilterDialog * dlg = new FilterDialog(wallet);
  dlg->show();
}

void WalletDW::findInternalMoves()
{
  if(wallet)
    wallet->findInternalMoves();
}

void WalletDW::findInternalMovesPermissive()
{
  if(wallet)
    wallet->findInternalMoves(true);
}
