/*
    curvesdisplay.cc: displaying list of Transactions
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
#include <curvesdisplay.hh>
#include <wallet.hh>



CurvesDisplay::CurvesDisplay()
{
  // First, a VBox:
  QVBoxLayout * l1 = new QVBoxLayout(this);
  topLabel = new QLabel();
  l1->addWidget(topLabel);

  curvesWidget = new TimeBasedWidget;
  l1->addWidget(curvesWidget);
  curvesWidget->setMinimumSize(QSize(400,200));


  QPushButton * bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(bt);

  // Force the deletion on close
  setAttribute(Qt::WA_DeleteOnClose);
}

CurvesDisplay::~CurvesDisplay()
{
}


void CurvesDisplay::displayBalance(const TransactionList * transactions, 
                                   const QColor & col)
{
  // We must assumed that they are sorted to some extent. The last
  // transaction for one day is the balance for that day.

  TimeBasedCurve * c = new TimeBasedCurve(col);
  
  QDate last;
  int lastBalance;
  for(int i = 0; i < transactions->size(); i++) {
    const Transaction * t = &transactions->value(i);
    if(last.isValid() && t->getDate() != last)
      (*c) << DataPoint(last, lastBalance);
    last = t->getDate();
    lastBalance = t->getBalance();
  }
  (*c) << DataPoint(last, lastBalance);

  curvesWidget->addCurve(c);
}
