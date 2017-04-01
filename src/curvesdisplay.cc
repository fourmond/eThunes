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

  QHBoxLayout * hl = new QHBoxLayout();
  QPushButton * bt;
  bt = new QPushButton(tr("H zoom in"));
  connect(bt, SIGNAL(clicked()), SLOT(hZoomIn()));
  hl->addWidget(bt);
  bt = new QPushButton(tr("H zoom out"));
  connect(bt, SIGNAL(clicked()), SLOT(hZoomOut()));
  hl->addWidget(bt);
  bt = new QPushButton(tr("V zoom in"));
  connect(bt, SIGNAL(clicked()), SLOT(vZoomIn()));
  hl->addWidget(bt);
  bt = new QPushButton(tr("V zoom out"));
  connect(bt, SIGNAL(clicked()), SLOT(vZoomOut()));
  hl->addWidget(bt);
  bt = new QPushButton(tr("Auto"));
  hl->addWidget(bt);
  l1->addLayout(hl);

  hl = new QHBoxLayout();
  curvesWidget = new TimeBasedWidget;
  hl->addWidget(curvesWidget);
  curvesWidget->setMinimumSize(QSize(400,200));
  curvesWidget->connect(bt, SIGNAL(clicked()), SLOT(autoScale()));

  // Legends of the curves...
  checkBoxes = new QVBoxLayout();
  checkBoxes->addWidget(new QLabel(tr("<b>Legend</b>")));

  hl->addLayout(checkBoxes);
  cbGroup = new QButtonGroup;
  cbGroup->setExclusive(false);
  connect(cbGroup, SIGNAL(buttonClicked(int)), SLOT(cbClicked(int)));

  l1->addLayout(hl);

  bt = new QPushButton(tr("Close"));
  connect(bt, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(bt);

  // Force the deletion on close
  setAttribute(Qt::WA_DeleteOnClose);
}

CurvesDisplay::~CurvesDisplay()
{
}

TimeBasedCurve * CurvesDisplay::balanceForTransactionList(const TransactionList * transactions)
{
  // We must assumed that they are sorted to some extent. The last
  // transaction for one day is the balance for that day.

  TimeBasedCurve * c = new TimeBasedCurve();
  
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
  return c;
}


void CurvesDisplay::displayBalance(const TransactionList * transactions,
                                   const QString & str,
                                   QColor col)
{
  TimeBasedCurve * c = balanceForTransactionList(transactions);
  if(! col.isValid())
    col = curvesWidget->nextColor();
  c->style = CurveStyle(col);
  addCurve(c, str);
}

// We do two years
void CurvesDisplay::displayBalance(const Wallet * w, 
                                   const QColor & col)
{
  TimeBasedCurve * c = new TimeBasedCurve(col);
  
  QDate now = QDate::currentDate();
  QDate i = now.addYears(-2);
  while(i < now) {
    (*c) << DataPoint(i, w->balance(i));
    i = i.addDays(1);
  }
  addCurve(c);
}

void CurvesDisplay::addCurve(TimeBasedCurve * c, QString str)
{
  int id = curvesWidget->number();
  curvesWidget->addCurve(c);
  if(str.isEmpty())
    str = QString("Data %1").arg(id + 1);
  QPixmap px(20,20);
  px.fill(c->style.brush.color());
  QCheckBox * cb = new QCheckBox(str);
  cb->setChecked(true);
  cb->setIcon(QIcon(px));
  checkBoxes->addWidget(cb);
  cbGroup->addButton(cb, id);
}

/// @todo Fix this !
static const char * colors[] = 
  { "red", "blue", "#080", "orange", "purple"};
static int nbColors = sizeof(colors)/sizeof(colors[0]);

void CurvesDisplay::displayAllBalances(const Wallet * w)
{
  QList< QList<DataPoint> > curves;
  int nb = 0;

  QHash<const AccountGroup *, QList< QList<DataPoint> > > groups;

  for(int i = 0; i < w->accountGroups.size(); i++)
    groups[&w->accountGroups[i]] = QList< QList<DataPoint> >();
  
  for(int i = 0; i < w->accounts.size(); i++) {
    TimeBasedCurve * c = 
      balanceForTransactionList(&(w->accounts[i].transactions));
    c->style = CurveStyle(QColor(colors[nb++ % nbColors]));
    addCurve(c, w->accounts[i].name());
    curves << c->curveData();

    for(auto it = groups.begin(); it != groups.end(); it ++) {
      if(it.key()->contains(&w->accounts[i]))
        it.value() << c->curveData();
    }
  }

  if(curves.size() == 0)
    return;


  for(auto it = groups.begin(); it != groups.end(); it ++) {
    TimeBasedCurve * c = new TimeBasedCurve();
    c->style = CurveStyle(QColor(colors[nb++ % nbColors]));
    c->setToSum(it.value());
    addCurve(c, it.key()->name);
  }



  TimeBasedCurve * c = new TimeBasedCurve();
  c->style = CurveStyle(QColor(colors[nb++ % nbColors]));
  c->setToSum(curves);

  addCurve(c, "Total");
}



void CurvesDisplay::zoomIn(const QSizeF & zF)
{
  curvesWidget->zoomIn(zF);
}

void CurvesDisplay::hZoomIn(double fact)
{
  curvesWidget->zoomIn(QSizeF(fact, 1));
}

void CurvesDisplay::vZoomIn(double fact)
{
  curvesWidget->zoomIn(QSizeF(1, fact));
}

void CurvesDisplay::hZoomOut(double fact)
{
  curvesWidget->zoomIn(QSizeF(1/fact, 1));
}

void CurvesDisplay::vZoomOut(double fact)
{
  curvesWidget->zoomIn(QSizeF(1, 1/fact));
}

void CurvesDisplay::cbClicked(int id)
{
  curvesWidget->show(id, cbGroup->button(id)->isChecked());
}
