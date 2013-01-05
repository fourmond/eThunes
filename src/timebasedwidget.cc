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


class InnerWidget : public QWidget {

  TimeBasedWidget * timeBasedWidget;
protected:

  /// We want to be able to paint the widget how we like...
  virtual void paintEvent(QPaintEvent * event) {
    timeBasedWidget->paintMe(event);
  };
public:

  InnerWidget(TimeBasedWidget * tbw) : 
  timeBasedWidget(tbw) { ;};
  
};


/// @todo Reimplement all this to use QGraphicsScene and
/// background/foreground stuff ? If I play correctly this time, I
/// shouldn't have problems...
TimeBasedWidget::TimeBasedWidget(QWidget * w) : QScrollArea(w)
{
  pixelPerDay = 2;
  setWidget(new InnerWidget(this));
  setAlignment(Qt::AlignCenter);
}

void TimeBasedWidget::paintMe(QPaintEvent * ev)
{
  // An absolute must !
  // QPainter p(widget());
  QPainter p(widget());
  p.setRenderHints(QPainter::Antialiasing);

  QRect r = widget()->rect();

  // Translate the paint object so that 0 is at the bottom
  p.translate(QPoint(0, r.height() - 5));


  // First, paint the curves themselves

  /// @todo Setup clipping.
  for(int i = 0; i < curves.size(); i++)
    curves[i]->paint(&p, earliest, pixelPerDay, min, verticalScale);

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

  QSize s = viewport()->size();

  int newWidth = earliest.daysTo(latest) * pixelPerDay;
  verticalScale = (max - min)/(s.height() - 10); // 10, and what ?

  curves << curve;

  QSize ns(newWidth, s.height());
  widget()->setMinimumSize(ns);
  widget()->resize(ns);
}



TimeBasedWidget::~TimeBasedWidget()
{
  for(int i = 0; i < curves.size(); i++)
    delete curves[i];
}
