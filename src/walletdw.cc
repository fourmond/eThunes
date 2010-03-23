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

#include <navigationwidget.hh>
#include <accountpage.hh>

WalletDW::WalletDW(Wallet * w) : wallet(w)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);
  connect(summary, SIGNAL(linkActivated(const QString &)), 
	  SLOT(showURL(const QString &)));

  // A button bar at the bottom for importing/saving/loading.

  QHBoxLayout * hl = new QHBoxLayout();
  // The button for importing something into the wallet
  QPushButton * bt = new QPushButton(tr("Import OFX"));
  hl->addWidget(bt);
  connect(bt, SIGNAL(clicked()), SLOT(fileImportDialog()));

  bt = new QPushButton(tr("Save"));
  hl->addWidget(bt);
  connect(bt, SIGNAL(clicked()), SLOT(save()));

  bt = new QPushButton(tr("Load"));
  hl->addWidget(bt);
  connect(bt, SIGNAL(clicked()), SLOT(load()));

  layout->addLayout(hl);


  // We introduce contents into the summary.
  updateSummary();
}

void WalletDW::updateSummary()
{
  QString text = QString("<strong>") + tr("Wallet") + "</strong>\n<p>";
  QString cellStyle = " style='padding-right: 20px'";
  int totalBalance = 0; /// \todo Maybe this should go in Wallet ?
  
  text += "<table>\n";
  text += QString("<tr><th" + cellStyle +">%1</th><th>%2</th></tr>\n").
    arg(tr("Account")).arg(tr("Balance"));
  for(int i = 0; i < wallet->accounts.size(); i++) {
    Account * ac = &wallet->accounts[i];
    text += QString("<tr><td" + cellStyle +"><a href='%1'>").
      arg(i) + QString("%1</a></td><td align='right'>%2</td></tr>\n").
      arg(ac->name()).arg(Transaction::formatAmount(ac->balance()));
    totalBalance += ac->balance();
  }
  text += (QString("<tr><td>") + tr("Total") + "</td><td>%1</td></tr>\n").
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

/// \todo Here be careful if there are more than one kind of URLs one
/// could handle.
void WalletDW::showURL(const QString & link)
{
  int id = link.toInt();
  AccountPage * page = new AccountPage(&wallet->accounts[id]);
  NavigationWidget::openUpNewPage(page);
}

void WalletDW::save()
{
  if(lastFilename.isEmpty()) {
    lastFilename = 
      QFileDialog::getSaveFileName(this, 
				   tr("Save wallet as"),
				   QString(),
				   tr("XML wallet files (*.xml)"));
    if(lastFilename.isEmpty())
      return;
  }
  wallet->saveToFile(lastFilename);
}

void WalletDW::load()
{
  QString file = 
    QFileDialog::getOpenFileName(this, 
				 tr("Select wallet to load"),
				 QString(),
				 tr("XML wallet files (*.xml)"));
  if(file.isEmpty())
    return;
  wallet->loadFromFile(file);
  /// \todo: here, we should find a way to invalidate all the windows
  /// that depend on this.
		       
  updateSummary();
}
