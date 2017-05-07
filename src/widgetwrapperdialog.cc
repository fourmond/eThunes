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


/// @todo Should be able to configure two things:
/// - whether there is a OK button
/// - and whether delete occurs on close
WidgetWrapperDialog::WidgetWrapperDialog(QWidget * widget,
                                         const QString & top,
                                         QDialogButtonBox::StandardButtons bts,
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


  buttons = new QDialogButtonBox(bts);
  l1->addWidget(buttons);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

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
