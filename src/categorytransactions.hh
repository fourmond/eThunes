/** 
    \file categorytransactions.hh
    A small dialog box to display the transactions that belong to a category
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


#ifndef __CATEGORYTRANSACTIONS_HH
#define __CATEGORYTRANSACTIONS_HH

#include <transaction.hh>
#include <category.hh>
#include <wallet.hh>

class CategoryTransactions : public QDialog {
  Q_OBJECT;

  /// The target Category
  Category * category;

  /// The target Wallet
  Wallet * wallet;

  TransactionPtrList list;
  
public:
  CategoryTransactions(Wallet * w, Category * c);

};

#endif
