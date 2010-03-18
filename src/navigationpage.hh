/** 
    \file navigationpage.hh
    A page in the navigation widget
    Copyright 2010 by Vincent Fourmond

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

#ifndef __NAVIGATIONPAGE_HH
#define __NAVIGATIONPAGE_HH

/// All pages to appear in the NavigationWidget must be children of
/// this base class.
class NavigationPage : public QWidget {

  Q_OBJECT;

public:
  
  /// Returns the title of the page
  virtual QString pageTitle() = 0;
  
  virtual ~NavigationPage();
  
};

#endif
