/*
  timebasedwidget.cc: Implementation of time based widgets
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
#include <timebasedwidget.hh>
#include <timebasedcurve.hh>

TimeBasedWidget::TimeBasedWidget(QWidget * w) : QWidget(w)
{
  pixelPerDay = 2;
}

void TimeBasedWidget::paintEvent(QPaintEvent * ev)
{
  QPainter p(this);

  QRect r = rect();

  double pad = 6;

  // First, we try to be clever about scaling the things...
  double yscale = (max - min)/(r.height() - pad);

  // Translate the paint object so that 0 is at the bottom
  p.translate(QPoint(0, r.height() - pad));


  // First, paint the curves themselves

  /// @todo Setup clipping.
  for(int i = 0; i < curves.size(); i++)
    curves[i]->paint(&p, earliest, pixelPerDay, min, yscale);

  // Then, legends, and the like ?
}


void TimeBasedWidget::addCurve(TimeBasedCurve * curve)
{
  if(curve->earliestPoint() < earliest || (! earliest.isValid()))
    earliest = curve->earliestPoint();

  if(curve->latestPoint() > latest || (! latest.isValid()))
    latest = curve->latestPoint();

  if(curves.size() == 0 || min > curve->minimumValue())
    min = curve->minimumValue();

  if(curves.size() == 0 || max < curve->maximumValue())
    max = curve->maximumValue();

  QSize s = size();
  
  int newWidth = earliest.daysTo(latest) * pixelPerDay;

  curves << curve;
  
  resize(newWidth, s.height());

  
}

TimeBasedWidget::~TimeBasedWidget()
{
  for(int i = 0; i < curves.size(); i++)
    delete curves[i];
}
