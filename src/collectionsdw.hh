/**
    \file collectionsdw.hh
    DashboarWidget for a list of collections
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

#ifndef __COLLECTIONSDW_HH
#define __COLLECTIONSDW_HH

#include <dashboardwidget.hh>
#include <cabinet.hh>

/// A dashboard widget to display all Collection objects.
///
/// \todo it would be very good to have a filesystem-like model to
/// display all the documents handled by a Cabinet, ordered by
/// canonicalFileName, with links to the collection, the PDF file, and
/// with the relevant data (in particular the display name).
class CollectionsDW : public DashboardWidget {

  Q_OBJECT;

  /// The Cabinet whose Collections we should display
  Cabinet * cabinet;

  /// The QLabel object displaying the rich text.
  QLabel * summary;

public:
  CollectionsDW(Cabinet * c);
  virtual ~CollectionsDW();

signals:

public slots:
  /// Updates the summary; should be called whenever the status of the
  /// wallet has changed somehow.
  void updateSummary();

  // /// Fires up a dialog box to import a given file (or more !) into
  // /// the wallet.
  // void fileImportDialog();

  /// Called internally when a link is clicked
  void showURL(const QString & link);

  /// Fires up a dialog box to add a new Collection
  void addCollectionDialog();

  // /// Manage  filters
  // void manageFilters();

  /// Pops up a dialog box to add documents to the given Collection
  void addDocumentsDialog(Collection * coll);

};

#endif
