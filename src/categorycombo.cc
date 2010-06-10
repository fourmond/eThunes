/*
    categorycombo.cc:  A combo box for editing Categories
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
#include <categorycombo.hh>

CategoryCombo::CategoryCombo(Wallet * w, QWidget * parent) :
  QComboBox(parent), wallet(w)
{
  setEditable(true);
}

void CategoryCombo::showPopup()
{
  if(wallet) {
    QString current = currentText();
    clear();
    QStringList names = wallet->categories.categoryNames();
    names.sort();
    addItems(names);
    setEditText(current);
  }
  QComboBox::showPopup();
}
