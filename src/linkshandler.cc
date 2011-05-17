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
#include <filterpage.hh>

#include <transactionlistdialog.hh>

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

QString LinksHandler::linkToMonthlyTransactions(Account * account, int monthID, 
                                                const QString & str)
{
  return wrapLink(QString("account-month:%1:%2").
                  arg(encodePointer(account)).
                  arg(monthID), str);
}

QString LinksHandler::linkToMonthlyCategoryTransactions(Category * category,
                                                Account * account, 
                                                int monthID, 
                                                const QString & str)
{
  return wrapLink(QString("category-month:%1:%2:%3").
                  arg(encodePointer(account)).
                  arg(monthID).
                  arg(encodePointer(category)), str);
}


/// @todo This is starting to seriously lack object-orientedness
/// here. Ideally, I should write up a whole hierarchy of classes and
/// rely on virtual functions. A side effect, though: the pointers
/// would have to be stored somewhere, if only for the sake of knowing
/// that if we want to free them, it's possible ;-)... Another thing
/// is that if I use a single interface, I could use a single
/// protocol, and have it registered by Qt. It would be easier to
/// support tooltips too.
///
/// @todo the name is confusing, because Link and LinksHandler are two
/// different things, even if they can be connected sometimes.
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
    page = FilterPage::getFilterPage(decodePointer<Wallet>(target));
  else if(protocol == "account-month") {
    QStringList a = target.split(":");
    Account * account = decodePointer<Account>(a[0]);
    int monthID = a[1].toInt();
    TransactionListDialog * dlg = new TransactionListDialog();
    dlg->displayMonthlyTransactions(account, monthID);
    widget = dlg;
  }
  else if(protocol == "category-month") {
    QStringList a = target.split(":");
    Account * account = decodePointer<Account>(a[0]);
    int monthID = a[1].toInt();
    Category * category = decodePointer<Category>(a[2]);
    TransactionListDialog * dlg = new TransactionListDialog();
    dlg->displayMonthlyCategoryTransactions(category,
                                            account, monthID);
    widget = dlg;
  }
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
