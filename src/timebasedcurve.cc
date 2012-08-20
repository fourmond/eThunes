/*
  timebasedcurve.cc: Implementation of time based curves
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

#include <headers.hh>
#include <timebasedcurve.hh>

QDate DataPoint::date() const
{
  return d;
}

QString DataPoint::toolTip() const
{
  return tT;
}

int DataPoint::amount() const
{
  return a;
}

DataPoint::DataPoint(const QDate & da, int am, 
                     const QString tt) :
  d(da), tT(tt), a(am) {
}

bool DataPoint::operator<(const DataPoint & other) const
{
  return d < other.d;
}
