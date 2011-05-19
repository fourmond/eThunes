/*
    cabinetpage.cc: Implementation of CabinetPage
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
#include <cabinetpage.hh>

#include <statistics.hh>
#include <linkshandler.hh>

CabinetPage::CabinetPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);

  QHBoxLayout * hb = new QHBoxLayout();
  walletDW = new WalletDW(&cabinet->wallet);
  hb->addWidget(walletDW);

  collectionsDW = new CollectionsDW(cabinet);
  hb->addWidget(collectionsDW);

  layout->addLayout(hb);


  stats = new QLabel();
  statsArea = new QScrollArea;
  statsArea->setWidget(stats);

  layout->addWidget(statsArea);
  LinksHandler::handleObject(stats);
  stats->show();
  

  updateContents();
  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SLOT(updateContents()));

  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SIGNAL(filenameChanged(const QString&)));
  stats->show();
}


CabinetPage::~CabinetPage()
{
}

QString CabinetPage::pageTitle()
{
  return tr("Dashboard");
}

void CabinetPage::updateContents()
{
  summary->setText(tr("<b>Cabinet : </b>%1<p>").
		   arg(cabinet->fileName()));
  if(cabinet->wallet.accounts.size() > 0) {
    // We pick the one with the most transactions:
    Account * account = NULL;
    for(int i = 0; i < cabinet->wallet.accounts.size(); i++) {
      if( ( ! account) ||
          account->transactions.size() < 
          cabinet->wallet.accounts[i].transactions.size())
        account = &cabinet->wallet.accounts[i];
      Statistics s(account->transactions.toPtrList());
      stats->setText(s.htmlStatistics(16));
      stats->resize(stats->sizeHint());
    }
  } 
  else
    stats->setText("Stuff !");
}

void CabinetPage::save()
{
  if(currentFileName().isEmpty())
    saveAs();
  else
    cabinet->saveToFile(currentFileName());
}

void CabinetPage::saveAs()
{
  QString str =
    QFileDialog::getSaveFileName(this,
				 tr("Save cabinet as"),
				 QString(),
				 tr("XML cabinet files (*.xml)"));
  if(str.isEmpty())
    return;
  cabinet->saveToFile(str);
}

void CabinetPage::load()
{
  QString file =
    QFileDialog::getOpenFileName(this,
				 tr("Select cabinet to load"),
				 QString(),
				 tr("XML cabinet files (*.xml)"));
  if(file.isEmpty())
    return;
  load(file);
}

void CabinetPage::load(const QString & file)
{
  cabinet->loadFromFile(file);
}
