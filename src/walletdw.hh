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

  /// The last filename used for saving
  QString lastFilename;

public:
  WalletDW(Wallet * w);
  virtual ~WalletDW();

  const QString & currentFileName() const {
    return lastFilename;
  }

public slots:
  /// Updates the summary; should be called whenever the status of the
  /// wallet has changed somehow.
  void updateSummary();

  /// Fires up a dialog box to import a given file (or more !) into
  /// the wallet.
  void fileImportDialog();

  /// Called internally when a link is clicked
  void showURL(const QString & link);

  /// Saves the Wallet.
  void save();

  /// Saves the Wallet into a new file.
  void saveAs();

  /// Prompts to load a file
  void load();

  /// Loads a file
  void load(const QString & file);

  /// Manage  filters
  void manageFilters();

};

#endif
