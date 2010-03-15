/** 
    \file wallet.hh
    Class representing a collection of accounts.
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


#ifndef __WALLET_HH
#define __WALLET_HH

#include <account.hh>
#include <ofximport.hh>

/// This class represents a collection of accounts. Presumably, there
/// should be only one Wallet instance in a running program, although
/// I probably shouldn't rely on this assumption.
///
/// \todo maybe this should be a Q_OBJECT sending signals when
/// information has changed ?
class Wallet {
public:
  /// The accounts held within the wallet.
  QList<Account> accounts;

  /// Import all the contents of an OFX import or something similar
  /// into the wallet. That means import the accounts and the
  /// transactions if necessary.
  void importAccountData(const OFXImport & data);
};

#endif
