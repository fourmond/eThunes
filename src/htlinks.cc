/*
  htlinks.cc: implementation of the followLink virtual functions
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

/// \file htlinks.cc The purpose of this file is to take out all
/// "graphical" functions from the non-graphical objects
/// (transactions, documents). The former are necessary for the links,
/// but rely on graphical headers which have no place in the original code.

#include <headers.hh>

#include <navigationwidget.hh>

#include <collection.hh>
#include <collectionpage.hh>

#include <account.hh>
#include <accountpage.hh>

void Collection::followLink()
{
  NavigationWidget::gotoPage(CollectionPage::getCollectionPage(this));
}

void Account::followLink()
{
  NavigationWidget::gotoPage(AccountPage::getAccountPage(this));
}


void Transaction::followLink()
{
  if(! account)
    return;                     /// @todo Should log error
  AccountPage * acPage =
    AccountPage::getAccountPage(account);
  acPage->showTransaction(this);
  NavigationWidget::gotoPage(acPage);
}

void Document::followLink()
{
  if(! collection)
    return;
  NavigationWidget::gotoPage(CollectionPage::getCollectionPage(collection));
}
