/** 
    \file accountchecks.hh
    A small dialog box to display the cheques from an account
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


#ifndef __ACCOUNTCHECKS_HH
#define __ACCOUNTCHECKS_HH

#include <transaction.hh>
#include <account.hh>

/// Dialog box to display checks
///
/// \todo This class should be merged with CategoryTransactions to
/// make a single dialog box (either using only one class, through
/// specific functions setting the TransactionPtrList and the label
/// text, or using derived class of a single class, but this seems
/// less neat)
///
/// \todo In particular, the final class should handle resizing
/// properly.
class AccountChecks : public QDialog {
  Q_OBJECT;

  /// The target Account
  Account * account;

  TransactionPtrList list;
  
public:
  AccountChecks(Account * c);

};

#endif
