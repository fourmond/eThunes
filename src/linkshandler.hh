/**
    \file linkshandler.hh
    A class for handling all kinds of links
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

#ifndef __LINKSHANDLER_HH
#define __LINKSHANDLER_HH

class Account;
class AccountPage;
class Transaction;
class Wallet;
class Linkable;
class Category;

/// Handles all links by opening the right NavigationPage (or possibly
/// firing up dialog boxes ?)
class LinksHandler : public QObject {

  Q_OBJECT;

protected:
  /// Encodes a pointer into an internal representation
  static QString encodePointer(void * pointer);

  /// Decodes the internal representation back to a pointer
  template<typename T> static T * decodePointer(const QString & str);

  static LinksHandler * handler;



  /// Wraps the link into an <a> element if str isn't empty.
  static QString wrapLink(const QString &link,
			  const QString &str = QString());
public:

  static LinksHandler * getHandler();

  /// Attaches an object; it must have the linkActivated and
  /// linkHovered signals.
  void attachObject(QObject * object);

  /// Attaches an object; it must have the linkActivated and
  /// linkHovered signals.
  static void handleObject(QObject * object) {
    getHandler()->attachObject(object);
  };

  /// Returns the text for a link to an Account.
  static QString linkTo(Account * account, const QString & text = QString());

  /// Returns the text for a link to a Transaction.
  static QString linkTo(Transaction * transaction,
			const QString & text = QString());

  /// Returns the text for a link to a Linkable object.
  static QString linkTo(Linkable * link,
			const QString & text = QString());


  /// Returns the text for a link to the CategoryPage for the given
  /// Wallet
  static QString linkToCategories(Wallet * w, const QString & text = QString());

  /// Returns the text for a link to edit the filters for the given
  /// Wallet
  static QString linkToFilters(Wallet * w, const QString & text = QString());

  /// Link to a given month in the target account:
  static QString linkToMonthlyTransactions(Account * account, int monthID, 
                                           const QString & text = QString());

  /// Link to a given month in the target account:
  static QString linkToMonthlyCategoryTransactions(Category * category,
                                                   Account * account, 
                                                   int monthID, 
                                                   const QString & text = QString());
                                                         
public slots:

  void followLink(const QString & str);

signals:

  /// This signal is emitted in case of an unhandled link
  void unhandledLink(const QString & str);

};

#endif
