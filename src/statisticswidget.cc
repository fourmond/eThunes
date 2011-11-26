/*
    statisticswidget.cc: implementation of the StatisticsWidget class
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
#include <statisticswidget.hh>

#include <cabinet.hh>

StatisticsWidget::StatisticsWidget(Cabinet * c) : 
  cabinet(c), maxDisplayed(6)
{
  QVBoxLayout * layout = new QVBoxLayout(this);

  layout->addWidget(new QLabel(tr("<h2>Statistics:</h2>")));
  
  QHBoxLayout * horiz = new QHBoxLayout();
  horiz->addWidget(new QLabel(tr("<b>Number of displayed categories:</b>")));

  QSpinBox * sp = new QSpinBox();
  sp->setValue(maxDisplayed);
  sp->setSingleStep(1);
  connect(sp, SIGNAL(valueChanged(int)), SLOT(setDisplayed(int)));
  horiz->addWidget(sp);

  topLevel = new QCheckBox("Top level ?");
  topLevel->setChecked(true);
  connect(topLevel, SIGNAL(stateChanged(int)), SLOT(update()));
  horiz->addWidget(topLevel);

  layout->addLayout(horiz);

  
  display = new HTDisplay();
  layout->addWidget(display);
}


void StatisticsWidget::update()
{
  if(cabinet->wallet.accounts.size() > 0) {
    // We pick the one with the most transactions:
    Account * account = NULL;
    for(int i = 0; i < cabinet->wallet.accounts.size(); i++) {
      if( ( ! account) ||
          account->transactions.size() < 
          cabinet->wallet.accounts[i].transactions.size())
        account = &cabinet->wallet.accounts[i];
      Statistics s(account->transactions.toPtrList(),
                   topLevel->isChecked());
      display->setText(s.htmlStatistics(-1, maxDisplayed));
    }
  } 
  else
    display->setText("Stuff !");
}

void StatisticsWidget::setDisplayed(int nb)
{
  maxDisplayed = nb;
  update();
}
