/**
    \file documentwidget.hh
    A widget displaying a Document
    Copyright 2017 by Vincent Fourmond

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


#ifndef __DOCUMENTWIDGET_HH
#define __DOCUMENTWIDGET_HH

#include <document.hh>

class Cabinet;
class Document;

/// A widget displaying the contents of a document, possibly offering
/// edition of the document type
class DocumentWidget : public QWidget {
  Q_OBJECT;

  /// File name of the current 
  QString fileName;

  /// Document being displayed. Can be NULL, will be created when
  /// trying to modify anything.
  Document * document;

  /// First line (document path)
  QLabel * firstLine;

  /// Summary/description of the document
  QLabel * description;

  /// Combobox for changing types
  QComboBox * documentTypeCombo;

  /// The underlying Cabinet
  Cabinet * cabinet;

  /// Sets up the frame
  void setupFrame();

public:

  DocumentWidget(Cabinet * c, QWidget * parent = NULL);
  ~DocumentWidget();

public slots:

  /// Shows the given document.
  void showDocument(const QString & doc);

protected slots:
  /// Called on changing type
  void onTypeChanged(const QString & newType);
};


#endif
