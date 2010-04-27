/** 
    \file cabinetpage.hh
    A page displaying informations about the categories
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

#ifndef __CABINETPAGE_HH
#define __CABINETPAGE_HH

#include <cabinet.hh>
#include <navigationpage.hh>

#include <walletdw.hh>

/// This NavigationPage displays a Cabinet.
class CabinetPage : public NavigationPage {

  Q_OBJECT;

protected:


  /// The displayed Cabinet
  Cabinet * cabinet;


  QLabel * summary;

  /// The last filename used for saving
  QString lastFilename;

public:

  /// The widget displaying the Wallet. \todo maybe this shouldn't be
  /// a public attribute ?
  WalletDW * walletDW;

  const QString & currentFileName() const {
    return lastFilename;
  }

  
  CabinetPage(Cabinet * c);
  
  /// Returns the title of the page
  virtual QString pageTitle();
  
  virtual ~CabinetPage();

signals:

  /// Emitted whenever the document name is changed.
  void filenameChanged(const QString & filename);

public slots:

  /// Updates the page; useless for now ?
  void updateContents();

  /// Saves the Wallet.
  void save();

  /// Saves the Wallet into a new file.
  void saveAs();

  /// Prompts to load a file
  void load();

  /// Loads a file
  void load(const QString & file);

};

#endif
