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
  /// \todo Use something more suitable than this.
  closeTab = new QToolButton();
  closeTab->setIcon(QIcon("/usr/share/icons/gnome/24x24/actions/window-close.png"));
  closeTab->setAutoRaise(true);
  setCornerWidget(closeTab);

  connect(this, SIGNAL(currentChanged(int)), SLOT(tabChanged()));
  connect(closeTab, SIGNAL(clicked()), SLOT(closeCurrentTab()));
}

NavigationWidget::~NavigationWidget()
{
  // Unregister the widget if necessary
  if(theWidget == this)
    theWidget = 0;
}


void NavigationWidget::openUpNewPage(NavigationPage * page, bool gotoPage)
{
  theWidget->addPage(page);
  if(gotoPage)
    theWidget->setCurrentWidget(page);
}

void NavigationWidget::gotoPage(NavigationPage * page)
{
  if(theWidget->indexOf(page) >= 0)
    theWidget->setCurrentWidget(page);
  else 
    openUpNewPage(page, true);
}

/// \todo Probably a lot of bookkeeping could go inside here. (check
/// for duplicates, shift names, setup buttons for closing the
/// tabs...) Although maybe not?
void NavigationWidget::addPage(NavigationPage * page)
{
  addTab(page, page->pageTitle());
}

void NavigationWidget::tabChanged()
{
  closeTab->setEnabled(currentIndex() > 0);
}

void NavigationWidget::closeCurrentTab()
{
  if(currentIndex() > 0)
    removeTab(currentIndex());
}
