/*
    documentwidget.cc: implementation of the DocumentWidget class
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

#include <headers.hh>
#include <documentwidget.hh>

#include <document.hh>
#include <doctype.hh>
#include <cabinet.hh>

DocumentWidget::DocumentWidget(Cabinet * c, QWidget * parent) :
  QWidget(parent), document(NULL)
{
  setupFrame();
  showDocument(QString());
}

DocumentWidget::~DocumentWidget()
{
}


void DocumentWidget::setupFrame()
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  QHBoxLayout * hb = new QHBoxLayout;
  firstLine = new QLabel();
  hb->addWidget(firstLine);
  documentTypeCombo = new QComboBox;
  documentTypeCombo->setEditable(false);
  documentTypeCombo->addItem("(none)");
  QStringList doctypes = DocType::documentNames();
  std::sort(doctypes.begin(), doctypes.end());
  for(const QString & n : doctypes)
    documentTypeCombo->addItem(n);
  hb->addWidget(documentTypeCombo);
  layout->addLayout(hb);
  description = new QLabel();
  layout->addWidget(description);
}

void DocumentWidget::showDocument(const QString & str)
{
  firstLine->setText(tr("<b>Document: </b>%1").arg(str));
}
