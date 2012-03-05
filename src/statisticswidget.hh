/**
    \file statisticswidget.hh
    A widget displaying Statistics
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


#ifndef __STATISTICSWIDGET_HH
#define __STATISTICSWIDGET_HH

#include <statistics.hh>
#include <htlabel.hh>

class Cabinet;

/// A widget displaying statistics about transactions in a given
/// account.
///
/// @todo make provisions for displaying \b all transactions, should
/// that become interesting.
class StatisticsWidget : public QWidget {
  Q_OBJECT;

  /// The cabinet we're interested in
  Cabinet * cabinet;

  // /// And a way to make it scroll.
  // QScrollArea * statsArea;

  void setupFrame();

  /// The number of elements currently displayed in the stats.
  int maxDisplayed;

  /// The display of text
  HTDisplay * display;

  /// Whether we display only topLevel stuff or all
  QCheckBox * topLevel;

  /// Whether we display monthly average or raw information
  QCheckBox * monthlyAverage;

  /// The time frame of the statistics
  QComboBox * timeFrame;

public:

  StatisticsWidget(Cabinet * c);

public slots:
  void update();
  void setDisplayed(int nb);


  void setPeriod(int nb);
};


#endif
