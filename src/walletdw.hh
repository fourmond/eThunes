/**
    \file walletdw.hh
    DashboarWidget for the wallet class
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

#ifndef __WALLETDW_HH
#define __WALLETDW_HH

#include <dashboardwidget.hh>
#include <wallet.hh>

class WalletDW : public DashboardWidget {

  Q_OBJECT;

  /// The wallet whose contents we should display.
  Wallet * wallet;

  /// The QLabel object displaying the rich text.
  QLabel * summary;

public:
  WalletDW(Wallet * w);
  virtual ~WalletDW();


signals:

public slots:
  /// Updates the summary; should be called whenever the status of the
  /// wallet has changed somehow.
  void updateSummary();

  /// Fires up a dialog box to import a given file (or more !) into
  /// the wallet.
  void fileImportDialog();

  /// Manage  filters
  void manageFilters();

  /// Attemps to find internal moves, using
  /// TransactionPtrList::findInternalMoves()
  ///
  /// @todo To be improved later on.
  void findInternalMoves();

};

#endif
