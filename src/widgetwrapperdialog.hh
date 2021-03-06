/**
    \file widgetwrapperdialog.hh
    A generic dialog box wrapper for dialog boxes that
    essentially only display one widget (who cares itself about
    data persistence)
    Copyright 2011 by Vincent Fourmond

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


#ifndef __WIDGETWRAPPERDIALOG_HH
#define __WIDGETWRAPPERDIALOG_HH

/// Simple wrapper around one widget.
///
/// @warning By default, the WA_DeleteOnClose attribute is set. Use 
class WidgetWrapperDialog : public QDialog {
protected:
  Q_OBJECT;

  /// @todo This customization should be saved into using QSettings
  static QHash<QString, QByteArray> savedGeometries;

  QString internalName;

  void restoreGeometry();

  virtual void resizeEvent(QResizeEvent * event);

  QDialogButtonBox * buttons;

public:

  /// The label at the top of the dialog box
  QLabel * topLabel;

  /// Constructs
  WidgetWrapperDialog(QWidget * widget,
                      const QString & top = "",
                      QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok,
                      const QString & name = "",
                      bool wrap = false);

public slots:

  /// Displays the dialog box, disabling the WA_DeleteOnClose
  /// attribute first.
  int run();

protected slots:
  void saveGeometry();

};

#endif
