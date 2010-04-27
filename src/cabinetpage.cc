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
#include <cabinetpage.hh>

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
  updateContents();
  connect(this, SIGNAL(filenameChanged(const QString&)), 
	  SLOT(updateContents()));
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
		   arg(QFileInfo(lastFilename).fileName()));
}

void CabinetPage::save()
{
  if(lastFilename.isEmpty())
    saveAs();
  else
    cabinet->saveToFile(lastFilename);
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
  lastFilename = str;
  emit(filenameChanged(lastFilename));
  save();
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
  lastFilename = file;
  emit(filenameChanged(file));
}
