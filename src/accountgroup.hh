/**
    \file accountgroup.hh
    An easy way to group accounts
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


#ifndef __ACCOUNTGROUP_HH
#define __ACCOUNTGROUP_HH

#include <serializable.hh>

class Account;
class Wallet;

/// A group of accounts.
///
/// This class represents a group of accounts, for now for the simple
/// purpose of display. We may find other uses as time goes.
class AccountGroup : public Serializable {
  QList<Account *> accounts;

  /// List of strings representing accounts. To be only used at
  /// serialize time.
  QList<QString> accountStringList;
public:

  /// The display name of the account.
  QString name;

  AccountGroup() {;};

  virtual SerializationAccessor * serializationAccessor();

  virtual void prepareSerializationWrite();

  /// Function to be called on each group at the end of the
  /// serialization to ensure the pointers get correct.
  void finalizePointers(Wallet * wallet);

  /// The total balance of the group
  int balance();
};

#endif
