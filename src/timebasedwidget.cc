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

#include <transaction.hh>

#include <math.h>

/// @todo Reimplement all this to use QGraphicsScene and
/// background/foreground stuff ? If I play correctly this time, I
/// shouldn't have problems...
/// I'm not sure it's sooo nice after all, because QGraphicsScene is not
/// designed for objects whose line width is always the same pixel size.
TimeBasedWidget::TimeBasedWidget(QWidget * w) : 
QAbstractScrollArea(w), graphAreaMargins(80,5,5,20)
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

static double naturalDistances[] = { 1.0, 2.0, /*2.5,*/ 5.0, 10.0 };
const int nbNaturalDistances = sizeof(naturalDistances)/sizeof(double);

/// This function tries to picks ticks reasonably.
///
/// This code comes from Tioga's axes.c file.
static void pickMajorTicksLocation(double min, double max, 
                                   double * tick, double tickMin,
                                   double * fact,
                                   double * firstTick,
                                   double * lastTick,
                                   int * nb)
{
  /* The factor by which you need to divide to get
     the tick_min within [1,10[ */
  double factor = pow(10, floor(log10(tickMin)));
  double norm_tick_min = tickMin/factor;
  bool done = false;

  /* In principle, the loop below show run at most twice, but a
     safeguard is not too expensive ;-)... */
  int nb_tries = 0;
   
  do {
    nb_tries ++;
    int i;
    for(i = 0; i < nbNaturalDistances; i++)
      if(naturalDistances[i] >= norm_tick_min)
        break;
    /* Now, there is a corner case when there is not enough */

    *tick = naturalDistances[i] * factor;
      
    /* If the there is room for at most one tick here, there is a
       problem, so take the size down. */
    if( (max - min) < 2.0 * *tick) {
      factor = pow(10, floor(log10(tickMin/2)));
      norm_tick_min = tickMin/(2*factor);
    }
    else
      done = true;
  } while(! done && nb_tries < 3);

   *firstTick = ceil(min /(*tick)) * (*tick);
   *lastTick = floor(max /(*tick)) * (*tick);

   * nb = (int)round((*lastTick - *firstTick)/(*tick));
}

void TimeBasedWidget::paintEvent(QPaintEvent * ev)
{
  // An absolute must !
  // QPainter p(widget());
  QPainter p(viewport());
  p.setRenderHints(QPainter::Antialiasing);

  QRect r = viewport()->rect();


  QRect graphRect = adjustedRect(r, graphAreaMargins);

  double firstTick;
  double lastTick, tick, fact;
  int nbTicks;

  QSize gs = computeGraphSize();
  int vertPos = verticalScrollBar()->value();

  double ymin = min + verticalScale * (gs.height() - vertPos -
                                       graphRect.height());
  double ymax = min + verticalScale * (gs.height() - vertPos);
  pickMajorTicksLocation(ymin, ymax, &tick, 40 * verticalScale, 
                         &fact, &firstTick, &lastTick, &nbTicks);

  {
    // Setting up the coordinates and transforms for the graph area
    p.save();
    p.setClipRect(graphRect);
    p.translate(graphRect.topLeft());


  

    // Translate the paint object so that 0 is at the bottom
    p.translate(QPoint(-horizontalScrollBar()->value(), 
                       verticalScrollBar()->maximum() - 
                       vertPos + graphRect.height() 
                       - padding));

    // First, paint background lines
    {
      p.save();

      // Vertical lines
      QPen pen = QPen(QColor("gray"), 1.5, Qt::DashLine);
      p.setPen(pen);
      for(int i = 0; i < nbTicks; i++) {
        int pos = (firstTick - min + i * tick)/verticalScale;
        p.drawLine(0, -pos, gs.width(), -pos);
      }

      // Horizontal lines to come...
      
      p.restore();
    }

    // Then, paint the curves themselves

    for(int i = 0; i < curves.size(); i++)
      curves[i]->paint(&p, earliest, pixelPerDay, min, verticalScale);
    p.restore();
  }
  p.drawRect(graphRect);

  // Then, legends, and the like ?
  {
    p.save();


    // Translate the paint object so that 0 is at the bottom
    p.translate(QPoint(0, 
                       verticalScrollBar()->maximum() - 
                       vertPos + graphRect.height() 
                       - padding));


    for(int i = 0; i < nbTicks; i++) {
      int pos = (firstTick - min + i * tick)/verticalScale;
      int value = (firstTick + i * tick);
      QString str = Transaction::formatAmount(value);
      /// @todo more fancy text !
      p.drawText(5, -pos, str);
    }
    

    p.restore();
  }

  ///
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

// Updates the range to min <-> max - page, keeping the position
// constant (ie same ratio between)
static void updateSlider(QAbstractSlider * slider, 
                         int min, int max, int page)
{
  double value = slider->value();
  double oldRange  = slider->maximum() - slider->minimum();
  slider->setRange(min, max - page);
  slider->setPageStep(page);
  double newRange  = slider->maximum() - slider->minimum();
  slider->setValue(value * newRange / oldRange);
}

void TimeBasedWidget::updateSliders()
{
  QSize graph = computeGraphSize();
  QSize vp = adjustedSize(viewport()->size(), graphAreaMargins);
  
  updateSlider(horizontalScrollBar(), 0, graph.width(), vp.width());
  updateSlider(verticalScrollBar(), 0, graph.height(), vp.height());
}


void TimeBasedWidget::zoomIn(const QSizeF & zF)
{
  verticalScale /= zF.height();
  pixelPerDay *= zF.width();
  updateSliders();
  viewport()->update();
}
