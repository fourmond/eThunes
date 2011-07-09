/**
    \file htlabel.hh
    QLabel with additional hypertext features (links)
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

#ifndef __HTLABEL_HH
#define __HTLABEL_HH

#include <httarget.hh>

class HTLabel : public QLabel {

  Q_OBJECT;

  /// The list of HTTarget objects owned by this class, that must be
  /// freed upon destruction/text change.
  QList<HTTarget *> ownedTargets;

  /// Get rid of owned targets.
  void freeTargets();

  /// Returns the HTTarget corresponding to the url, or NULL if this
  /// isn't a ht: url.
  static HTTarget * targetFromUrl(const QString & url);

public:

  HTLabel(const QString & txt, QWidget * parent = NULL);
  HTLabel(QWidget * parent = NULL);


  virtual ~HTLabel();
  
public slots:

  /// Sets the text of the label, and take ownership of HTTarget
  /// referenced when appropriate.
  void setText(const QString & str);

protected slots:
  /// Handle the links
  void onLinkClicked(const QString & url);

};

#endif
