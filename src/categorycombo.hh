/**
    \file categorycombo.hh
    A combo box for editing Categories
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


#ifndef __CATEGORYCOMBO_HH
#define __CATEGORYCOMBO_HH

#include <wallet.hh>

/// A small widget to edit Categories
class CategoryCombo : public QComboBox {

  Q_OBJECT;

  /// The target wallet
  Wallet * wallet;

protected:

public:
  CategoryCombo(Wallet * w, QWidget * parent = 0);

  /// Reimplemented to populate with the proper list before firing up
  /// the popup
  virtual void showPopup();
};

#endif
