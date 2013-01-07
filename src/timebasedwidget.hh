/**
   \file timebasedwidget.hh
   A widget displaying values varying in time
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

#ifndef __TIMEBASEDWIDGET_HH
#define __TIMEBASEDWIDGET_HH

class TimeBasedCurve;

/// This widget displays series of TimeBasedCurves (or apparented ?)
class TimeBasedWidget : public QAbstractScrollArea {
protected:

  /// Displayed stuff...
  QList<TimeBasedCurve *> curves;

  virtual void paintEvent(QPaintEvent * event);

  /// The earliest date
  QDate earliest;

  /// The latest one
  QDate latest;

  /// The minimum value
  int min;
  
  /// The maximum value
  int max;

  /// Pixel padding around the graph
  int padding;

  /// Number of pixels to represent one day.
  double pixelPerDay;

  /// Vertical scale
  double verticalScale;

  /// Computes the overall size of the graph using the current
  /// horizontal and vertical scales.
  QSize computeGraphSize(bool includePadding = true) const;

  /// Update the scrollers range according to the overall size of the
  /// graph and the current size of the widget
  void updateSliders();

  virtual void resizeEvent(QResizeEvent * event);

  /// Margins around the "graph area" of the plot
  QMargins graphAreaMargins;
  
public:


  TimeBasedWidget(QWidget * parent = 0);

  virtual ~TimeBasedWidget();

public slots:
  /// Adds the given curve and updates the widget's size.
  void addCurve(TimeBasedCurve * curve);


  /// Zooms in by the given factors.
  void zoomIn(const QSizeF & zF);


};

#endif
