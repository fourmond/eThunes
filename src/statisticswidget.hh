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

  /// The display
  HTLabel * stats;

  /// And a way to make it scroll.
  QScrollArea * statsArea;


public:

  StatisticsWidget(Cabinet * c);
};


#endif
