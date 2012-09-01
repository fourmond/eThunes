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
//////////////////////////////////////////////////////////////////////

CurveStyle::CurveStyle(const QColor &c)
{
  pen = QPen(c, 3);
  brush = QBrush(c);
  markerPen = QPen(QColor("black"), 1);
}


//////////////////////////////////////////////////////////////////////

TimeBasedCurve::TimeBasedCurve(const CurveStyle & s) : 
  isSorted(false), style(s)
{
}

void TimeBasedCurve::ensureSorted()
{
  if(isSorted)
    return;
  qSort(data);
  // We cache min and max
  min = max = data.first().amount();

  for(int i = 1; i < data.size(); i++) {
    int a = data[i].amount();
    if(a < min)
      min = a;
    if(a > max)
      max = a;
  }

  isSorted = true;
}

QDate TimeBasedCurve::earliestPoint()
{
  ensureSorted();
  /// @tdexception handle empty list
  return data.first().date();
}

QDate TimeBasedCurve::latestPoint()
{
  ensureSorted();
  /// @tdexception handle empty list
  return data.last().date();
}

int TimeBasedCurve::minimumValue()
{
  ensureSorted();
  return min;
}

int TimeBasedCurve::maximumValue()
{
  ensureSorted();
  return max;
}


void TimeBasedCurve::paint(QPainter * dest, const QDate & origin, 
                           double pixelPerDay, double yOrg, 
                           double yScale)
{
  dest->save();
  dest->setPen(style.pen);

  QPoint last;
  for(int i = 0; i < data.size(); i++) {
    const DataPoint & dp = data[i];
    int x = origin.daysTo(dp.date()) * pixelPerDay;
    int y = (yOrg - dp.amount())/yScale;        // Not bad ?
    QPoint cur(x,y);
    if(i)
      dest->drawLine(last, cur);
    last = cur;
  }


  dest->setPen(style.markerPen);
  dest->setBrush(style.brush);

  for(int i = 0; i < data.size(); i++) {
    const DataPoint & dp = data[i];
    int x = origin.daysTo(dp.date()) * pixelPerDay;
    int y = (yOrg - dp.amount())/yScale;        // Not bad ?
    int hsz = 4;
    dest->drawEllipse(x-hsz,y-hsz, 2*hsz, 2*hsz);
  }
  dest->restore();
}

void TimeBasedCurve::addPoint(const DataPoint & dp)
{
  isSorted = false;
  data.append(dp);
}


TimeBasedCurve & TimeBasedCurve::operator<<(const DataPoint & dp)
{
  addPoint(dp);
  return *this;
}
