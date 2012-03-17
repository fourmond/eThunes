/**
    \file ootest.hh
    A test widget for OOModel
    Copyright 2012 by Vincent Fourmond

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


#ifndef __OOTEST_HH
#define __OOTEST_HH

#include <accountmodel.hh>
#include <modelitems.hh>


class OOModel;
/// A widget displaying a QTreeView based on an
class OOTest : public QWidget {
  Q_OBJECT;

  /// The view !
  QTreeView * view;

  /// The model
  OOModel * model;

public:
  OOTest(TransactionList * transactions);

  /// Test the model !
  static void test(TransactionList * transactions);

  ~OOTest();

};


#endif
