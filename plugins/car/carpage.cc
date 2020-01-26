/*
  carpage.cc: Implementation of CarPage
  Copyright 2011 by Vincent Fourmond

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
#include <widgetwrapperdialog.hh>
#include <utils.hh>
#include <transaction.hh>


#include <htlabel.hh>
#include <httarget-templates.hh>

#include "carpage.hh"

QHash<CarPlugin *, CarPage *> CarPage::carPages;

CarPage * CarPage::getCarPage(CarPlugin * plugin)
{
  if(! carPages.contains(plugin))
    carPages[plugin] = new CarPage(plugin);
  return carPages[plugin];
}


CarPage::CarPage(CarPlugin * p) : plugin(p)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);


  layout->addStretch();
  updatePage();
}

QString CarPage::pageTitle() 
{
  return tr("Car Plugin: %1").arg(plugin->getName());
}



void CarPage::updatePage()
{
  // Here, we do various things, basically to update the main summary.
  QString str;
  str += tr("<h1>Car: %1</h1>\n").
    arg(plugin->getName());
  summary->setText(str);
}
