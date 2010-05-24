/*
    linkshandler.cc: Implementation of LinksHandler
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
#include <linkshandler.hh>
#include <navigationwidget.hh>

#include <navigationpage.hh>
#include <accountpage.hh>
#include <categorypage.hh>
#include <filterdialog.hh>
#include <collectionpage.hh>

LinksHandler * LinksHandler::handler = NULL;

LinksHandler * LinksHandler::getHandler()
{
  if(!handler)
    handler = new LinksHandler();
  return handler;
}


void LinksHandler::attachObject(QObject * obj)
{
  connect(obj, SIGNAL(linkActivated(const QString &)),
	  SLOT(followLink(const QString &)));
}


QString LinksHandler::encodePointer(void * pointer)
{
  char buffer[50];
  ::snprintf(buffer, sizeof(buffer), "%p", pointer);
  return QString::fromAscii(buffer);
}

template<typename T> T * LinksHandler::decodePointer(const QString & str)
{
  void * ptr;
  ::sscanf((const char *)str.toAscii(), "%p", &ptr);
  return static_cast<T*>(ptr);
}

QString LinksHandler::wrapLink(const QString &link, const QString &str)
{
  if(str.isEmpty())
    return link;
  return QString("<a href='") + link + "'>" + str + "</a>";
}


QString LinksHandler::linkTo(Account * account, const QString & str)
{
  return wrapLink(QString("account:") + encodePointer(account), str);
}

QString LinksHandler::linkTo(Transaction * transaction, const QString & str)
{
  return wrapLink(QString("transaction:") + encodePointer(transaction), str);
}

QString LinksHandler::linkTo(Linkable * l, const QString & str)
{
  return wrapLink(l->typeName() + ":" + encodePointer(l), str);
}

void LinksHandler::followLink(const QString & str)
{
  int index = str.indexOf(":");
  QString protocol = str.left(index);
  QString target = str.mid(index + 1);
  NavigationPage * page = NULL;
  QWidget * widget = NULL;
  if(protocol == "account")
    page =
      AccountPage::getAccountPage(decodePointer<Account>(target));
  else if(protocol == "transaction") {
    Transaction * transaction = decodePointer<Transaction>(target);
    AccountPage * acPage =
      AccountPage::getAccountPage(transaction->account);
    acPage->showTransaction(transaction);
    page = acPage;
  }
  else if(protocol == "categories")
    page =
      CategoryPage::getCategoryPage(decodePointer<Wallet>(target));
  else if(protocol == "document") {
    Document * document = decodePointer<Document>(target);
    page =
      CollectionPage::getCollectionPage(document->collection);
  }
  else if(protocol == "filters")
    widget = new FilterDialog(decodePointer<Wallet>(target));
  else {
    emit(unhandledLink(str));
    return;
  }

  if(page)
    NavigationWidget::gotoPage(page);
  else if(widget)
    widget->show();
}

QString LinksHandler::linkToCategories(Wallet * w, const QString & str)
{
  return wrapLink(QString("categories:") + encodePointer(w), str);
}

QString LinksHandler::linkToFilters(Wallet * w, const QString & str)
{
  return wrapLink(QString("filters:") + encodePointer(w), str);
}
