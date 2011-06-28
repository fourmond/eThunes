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
class WidgetWrapperDialog : public QDialog {
protected:
  Q_OBJECT;

  /// @todo This customization should be saved into using QSettings
  static QHash<QString, QByteArray> savedGeometries;

  QString internalName;

  void restoreGeometry();

  virtual void resizeEvent(QResizeEvent * event);

public:

  /// The label at the top of the dialog box
  QLabel * topLabel;

  /// The close button at the bottom of the dialog box
  QPushButton * closeButton;

  /// Constructs
  WidgetWrapperDialog(QWidget * widget,
                      const QString & top = "",
                      const QString & close = "",
                      const QString & name = "");

protected slots:
  void saveGeometry();

};

#endif
