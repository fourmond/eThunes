/**
   \file timebasedcurve.hh
   A curve displaying values varying in time
   Copyright 2012 by Vincent Fourmond

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

#ifndef __TIMEBASEDCURVE_HH
#define __TIMEBASEDCURVE_HH

/// A single point to be displayed.
///
/// @todo That may become a virtual base class later on.
class DataPoint {
protected:

  /// The date
  QDate d;

  /// a text describing a tool tip
  QString tT;

  /// The value
  int a;
public:

  /// The date;
  QDate date() const;

  /// Returns a tool tip describing the point
  QString toolTip() const;

  /// Returns the amount corresponding to the point
  int amount() const;

  DataPoint(const QDate & date, int amount, 
            const QString toolTip = QString());

  /// For sorting by date
  bool operator<(const DataPoint & other) const;
};

/// A full time based curve
///
/// @todo How to integrate that ? I need a draw operation of some
/// kind, into an appropriate widget.
class TimeBasedCurve {
public:

  /// Data points
  QList<DataPoint> data;

};

#endif
