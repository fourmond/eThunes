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
#include <accountpage.hh>
#include <categorypage.hh>

#include <linkshandler.hh>

WalletDW::WalletDW(Wallet * w) : wallet(w)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);
  LinksHandler::handleObject(summary);
  // connect(summary, SIGNAL(linkActivated(const QString &)), 
  // 	  SLOT(showURL(const QString &)));

  connect(wallet, SIGNAL(accountsChanged()),
	  SLOT(updateSummary()));

  // We introduce contents into the summary.
  updateSummary();
}

void WalletDW::updateSummary()
{
  QString text = QString("<strong>") + tr("Wallet") + "</strong>\n<p>";
  QString cellStyle = " style='padding-right: 20px'";
  int totalBalance = 0; /// \todo Maybe this should go in Wallet ?

  text += LinksHandler::
    linkToCategories(wallet, tr("%1 categories").
		     arg(wallet->categories.categoryCount())) + "\n";

  text += LinksHandler::linkToFilters(wallet, tr("%1 filters").
				      arg(wallet->filters.count()))
    + "<p>\n";
  
  /// \todo Maybe the facility for building up tables should end up
  /// somewhere as global utilities ?
  text += "<table>\n";
  text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
    arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < wallet->accounts.size(); i++) {
    Account * ac = &wallet->accounts[i];
    text += QString("<tr><td" + cellStyle +">") +
      LinksHandler::linkTo(ac, ac->name()) + 
      QString("</td><td align='right'>%1</td></tr>\n").
      arg(Transaction::formatAmount(ac->balance()));
    totalBalance += ac->balance();
  }
  text += "<tr></tr>";
  text += (QString("<tr><td><strong>") + tr("Total") + 
	   "</strong></td><td><strong>%1</strong></td></tr>\n").
    arg(Transaction::formatAmount(totalBalance));
  text += "</table>\n";
  
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
