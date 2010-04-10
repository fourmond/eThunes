/** 
    \file navigationwidget.hh
    The widget handling the tab-based navigation.
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

#ifndef __NAVIGATIONWIDGET_HH
#define __NAVIGATIONWIDGET_HH

#include <navigationpage.hh>

/// This class will handle a firefox-like navigation. The trick is
/// about the opening of the new pages; where and how are the new
/// widgets created ? This is not trivial, for now.
class NavigationWidget : public QTabWidget {

  Q_OBJECT;

  static NavigationWidget * theWidget;

  /// The button at the top right to close tabs.
  QToolButton * closeTab;

protected slots:
  /// Internally used when the tab is changed. Could be used to change
  /// the name ???? (well, no, I think)
  void tabChanged();

public:

  /// Creates a NavigationWidget and registers it at the last one opened.
  NavigationWidget();
  virtual ~NavigationWidget();

  /// Adds a page to the NavigationWidget
  /// 
  /// \todo many interesting things to be done at a later time.
  void addPage(NavigationPage * page);

  /// Adds a page to the last opened NavigationWidget
  static void openUpNewPage(NavigationPage * page, bool gotoPage = true);

  /// Goto the given page in the last opened NavigationWidget, or
  /// create it if it doesn't exist yet.
  static void gotoPage(NavigationPage * page);


  /// Gets the current NavigationPage
  /// Commented out as dangerous for now.
  // 
  // NavigationPage * currentPage() const { return 
  //     static_cast<NavigationPage *>(currentWidget());}  ;

public slots:
  /// Closes the current tab. Won't work for the first tab.
  void closeCurrentTab();

  /// Switches to next tab
  void switchToNextTab();

  /// Switches to next tab
  void switchToPreviousTab();

};

#endif
