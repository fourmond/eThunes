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
TimeBasedWidget::TimeBasedWidget(QWidget * w) : 
QAbstractScrollArea(w), graphAreaMargins(20,5,5,20)
{
  pixelPerDay = 2;
  padding = 5;
}

static QRect adjustedRect(const QRect & r, const QMargins & m)
{
  return r.normalized().adjusted(m.left(), m.top(), 
                                 -m.right(), -m.bottom());
}

static QSize adjustedSize(const QSize & s, const QMargins & m)
{
  return s - QSize(m.left() + m.right(),
                   m.top() + m.bottom());
}

void TimeBasedWidget::paintEvent(QPaintEvent * ev)
{
  // An absolute must !
  // QPainter p(widget());
  QPainter p(viewport());
  p.setRenderHints(QPainter::Antialiasing);

  QRect r = viewport()->rect();


  QRect graphRect = adjustedRect(r, graphAreaMargins);

  {
    // Setting up the coordinates and transforms for the graph area
    p.save();
    p.setClipRect(graphRect);
    p.translate(graphRect.topLeft());


  

    // Translate the paint object so that 0 is at the bottom
    p.translate(QPoint(-horizontalScrollBar()->value(), 
                       verticalScrollBar()->maximum() - 
                       verticalScrollBar()->value() + graphRect.height() 
                       - padding));

    //

    // First, paint the curves themselves

    /// @todo Setup clipping.
    for(int i = 0; i < curves.size(); i++)
      curves[i]->paint(&p, earliest, pixelPerDay, min, verticalScale);
    p.restore();
  }
  p.drawRect(graphRect);

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
  verticalScale = (max - min)/(s.height() - padding * 2); // 10, and what ?

  curves << curve;
  updateSliders();
}



TimeBasedWidget::~TimeBasedWidget()
{
  for(int i = 0; i < curves.size(); i++)
    delete curves[i];
}

QSize TimeBasedWidget::computeGraphSize(bool includePadding) const
{
  QSize sz(earliest.daysTo(latest) * pixelPerDay, 
           (max - min) / verticalScale);
  if(includePadding)
    sz += QSize(padding * 2, padding * 2);
  return sz;
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
}

void TimeBasedWidget::updateSliders()
{
  QSize graph = computeGraphSize();
  QSize vp = adjustedSize(viewport()->size(), graphAreaMargins);
  
  updateSlider(horizontalScrollBar(), 0, graph.width(), vp.width());
  updateSlider(verticalScrollBar(), 0, graph.height(), vp.height());
}
