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


/// @todo Reimplement all this to use QGraphicsScene and
/// background/foreground stuff ? If I play correctly this time, I
/// shouldn't have problems...
/// I'm not sure it's sooo nice after all, because QGraphicsScene is not
/// designed for objects whose line width is always the same pixel size.
TimeBasedWidget::TimeBasedWidget(QWidget * w) : QAbstractScrollArea(w)
{
  pixelPerDay = 2;
}

void TimeBasedWidget::paintEvent(QPaintEvent * ev)
{
  // An absolute must !
  // QPainter p(widget());
  QPainter p(viewport());
  p.setRenderHints(QPainter::Antialiasing);

  QRect r = viewport()->rect();

  // Translate the paint object so that 0 is at the bottom
  p.translate(QPoint(-horizontalScrollBar()->value(), 
                     verticalScrollBar()->maximum() - 
                     verticalScrollBar()->value() + r.height() - 5));

  //

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

  // QSize s = viewport()->size(); // Not sure the viewport is defined now.
  QSize s = size();
  verticalScale = (max - min)/(s.height() - 10); // 10, and what ?

  curves << curve;
  updateSliders();
}



TimeBasedWidget::~TimeBasedWidget()
{
  for(int i = 0; i < curves.size(); i++)
    delete curves[i];
}

QSize TimeBasedWidget::computeGraphSize() const
{
  return QSize(earliest.daysTo(latest) * pixelPerDay, 
               (max - min) / verticalScale);
}


void TimeBasedWidget::resizeEvent(QResizeEvent * event)
{
  updateSliders();
}

// Updates the range to min <-> max - page, keeping the value
// constant.
static void updateSlider(QAbstractSlider * slider, 
                         int min, int max, int page)
{
  slider->setRange(min, max - page);
  slider->setPageStep(page);
  QTextStream o(stdout);
  o << slider 
    << "\tmin: " << min 
    << "\tmax: " << max  
    << "\tpage: " << page << endl;
}

void TimeBasedWidget::updateSliders()
{
  QSize graph = computeGraphSize();
  QSize vp = viewport()->size();
  
  updateSlider(horizontalScrollBar(), 0, graph.width(), vp.width());
  updateSlider(verticalScrollBar(), 0, graph.height(), vp.height());
}
