/**
    \file curvesdisplay.hh
    A small generic dialog box to display list of pointers
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


#ifndef __CURVESDISPLAY_HH
#define __CURVESDISPLAY_HH

#include <timebasedwidget.hh>
#include <timebasedcurve.hh>


class TransactionList;
class Wallet;

/// Display one or more curves.
///
/// Comes in many different varieties
class CurvesDisplay : public QDialog {
protected:
  Q_OBJECT;

  QLabel * topLabel;

  /// The chart
  QChart * chart;

  /// The chart view
  QChartView * view;

  /// The X axis
  QDateTimeAxis * axisX;

  /// The Y axis
  QValueAxis * axisY;

  
  // /// The widget for displaying the curves
  // TimeBasedWidget * curvesWidget;

  TimeBasedCurve * balanceForTransactionList(const TransactionList * transactions);

  QLineSeries * seriesForTransactionList(const TransactionList * transactions) const;

  QVBoxLayout * checkBoxes;

  QButtonGroup * cbGroup;

public:
  CurvesDisplay();

  virtual ~CurvesDisplay();

public slots:

  //  void addCurve(TimeBasedCurve * c, QString str = "");

  /// Adds a curve
  void addCurve(QXYSeries * curve, const QString & legend = "");

  /// Setup the display to show the balance
  void displayBalance(const TransactionList * transactions,
                      const QString & str = "",
                      QColor col = QColor());
  
  /// Setup the display to show the overall balance
  void displayBalance(const Wallet * wallet, 
                      const QColor & col = QColor("red"));

  /// Setup the display to show balance of all accounts and overall
  /// balance too
  void displayAllBalances(const Wallet * wallet);

protected slots:

};

#endif
