/**
    \file documentlistwidget.hh
    A page displaying more information about an account
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

#ifndef __DOCUMENTLISTWIDGET_HH
#define __DOCUMENTLISTWIDGET_HH

class Document;
class FlowLayout;

/// A widget representing a single Document.
class DocumentWidget : public QFrame {
  Q_OBJECT;
  /// The target !
  Document * document;

  /// The label representing the document.
  QLabel * contents;

public:
  DocumentWidget(Document * doc);

protected slots:
  void updateContents();

  void followLink(const QString & url);

  void promptForFileAttachment();

protected:
  /// Redefined to emit a signal upon focusing
  void focusInEvent(QFocusEvent * event);

signals:
  void documentSelected(Document * doc);

};

/// Hmmm, this is not too bad, but probably should handle straight
/// away the scroll bars ? (no, I think the main page should)
///
/// @todo Maybe wrap the labels in a frame to have selectable
/// documents ? That would be neat and allow to signal which document
/// is selected.
class DocumentListWidget : public QWidget {

  Q_OBJECT;

  /// Handling the internal layout.
  FlowLayout * layout;

  /// Holds the correspondance document -> label.
  ///
  /// @todo is that really necessary ?
  QHash<Document *, DocumentWidget *> widgets;

  /// Essentially sets up the layout.
  void setupFrame();

  /// Clears up all items currently displayed.
  void clearItems();

public:

  DocumentListWidget(QWidget * parent = 0);
  DocumentListWidget(const QList<Document*> & documents,
                     QWidget * parent = 0);

  void showDocuments(const QList<Document*> & documents);
  ~DocumentListWidget();

};

#endif
