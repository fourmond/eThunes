/*
    navigationwidget.cc: Implementation of NavigationWidget
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

#include <headers.hh>
#include <navigationwidget.hh>


NavigationWidget * NavigationWidget::theWidget = 0;

NavigationWidget::NavigationWidget()
{
  theWidget = this;
}

NavigationWidget::~NavigationWidget()
{
  // Unregister the widget if necessary
  if(theWidget == this)
    theWidget = 0;
}


void NavigationWidget::openUpNewPage(NavigationPage * page)
{
  theWidget->addPage(page);
}

/// \todo Probably a lot of bookkeeping could go inside here. (check
/// for duplicates, shift names, setup buttons for closing the
/// tabs...) Although maybe not?
void NavigationWidget::addPage(NavigationPage * page)
{
  addTab(page, page->pageTitle());
}
