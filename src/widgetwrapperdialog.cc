/*
    transactionlistdialog.cc: displaying list of Transactions
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
#include <widgetwrapperdialog.hh>

QHash<QString, QByteArray> WidgetWrapperDialog::savedGeometries;


WidgetWrapperDialog::WidgetWrapperDialog(QWidget * widget,
                                         const QString & top,
                                         const QString & close,
                                         const QString & name, 
                                         bool wrap)
  : internalName(name)
{
  // First, a VBox:
  QVBoxLayout * l1 = new QVBoxLayout(this);
  topLabel = new QLabel(top);
  l1->addWidget(topLabel);
  if(wrap) {
    QScrollArea * sa = new QScrollArea;
    sa->setWidget(widget);
    l1->addWidget(sa, 1);
  }
  else 
    l1->addWidget(widget);

  QString str;
  if(close.isEmpty())
    str = tr("Close");
  else
    str = close;
  closeButton = new QPushButton(str);
  connect(closeButton, SIGNAL(clicked()), SLOT(close()));
  l1->addWidget(closeButton);

  // Force the deletion on close
  setAttribute(Qt::WA_DeleteOnClose);
  restoreGeometry();
}


void WidgetWrapperDialog::saveGeometry()
{
  if(! internalName.isEmpty())
    savedGeometries[internalName] = QWidget::saveGeometry();
}

void WidgetWrapperDialog::restoreGeometry()
{
  if(savedGeometries.contains(internalName))
    QWidget::restoreGeometry(savedGeometries[internalName]);
}

void WidgetWrapperDialog::resizeEvent(QResizeEvent * event)
{
  QDialog::resizeEvent(event);
  saveGeometry();
}
