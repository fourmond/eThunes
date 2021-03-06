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
#include <attributehashwidget.hh>

#include <poppler/qt5/poppler-qt5.h>
#include <memory>


DocumentWidget::DocumentWidget(Cabinet * c, QWidget * parent) :
  QWidget(parent), document(NULL), cabinet(c)
{
  setupFrame();
  showDocument(QString());
}

DocumentWidget::~DocumentWidget()
{
}


void DocumentWidget::setupFrame()
{
  QWidget * left = new QWidget;
  QVBoxLayout * layout = new QVBoxLayout(left);
  QHBoxLayout * hb = new QHBoxLayout;
  firstLine = new QLabel();
  hb->addWidget(firstLine, 1);

  documentTypeCombo = new QComboBox;
  documentTypeCombo->setEditable(false);
  documentTypeCombo->addItem("(none)");
  QStringList doctypes = DocType::documentNames();
  std::sort(doctypes.begin(), doctypes.end());
  for(const QString & n : doctypes)
    documentTypeCombo->addItem(n);
  hb->addWidget(documentTypeCombo);

  connect(documentTypeCombo, SIGNAL(currentTextChanged(const QString &)),
          SLOT(onTypeChanged(const QString &)));

  QPushButton * button = new QPushButton(tr("Autodetect"));
  QObject::connect(button, &QAbstractButton::clicked, [this](bool) {
      if(! document)
        document = cabinet->documents.modifiableDocument(fileName);
      document->autoDetectDocType();
      showDocument(fileName);
    }
    );
  hb->addWidget(button);
  layout->addLayout(hb);
  
  description = new QLabel();
  layout->addWidget(description);

  attributesEditor = new AttributeHashWidget;
  layout->addWidget(attributesEditor);

  layout->addStretch(1);

  splitter = new QSplitter;
  splitter->addWidget(left);
  layout = new QVBoxLayout(this);
  layout->addWidget(splitter);

  // Display code
  pageArea = new QScrollArea;

  

  pageDisplay = new QLabel;
  pageDisplay->setBackgroundRole(QPalette::Base);

  pageArea->setBackgroundRole(QPalette::Dark);
  pageArea->setWidget(pageDisplay);

  splitter->addWidget(pageArea);
  
}

void DocumentWidget::showPage(int page)
{
  QString af = Cabinet::globalCabinet()->
    baseDirectory().absoluteFilePath(fileName);
  std::unique_ptr<Poppler::Document> doc(Poppler::Document::load(af));
  QTextStream o(stdout);
  if(! doc || doc->isLocked()) {
    o << "Failed to load document: " << af << endl;
    return;
  }
  std::unique_ptr<Poppler::Page> pdfPage(doc->page(page));
  // Document starts at page 0
  if(! pdfPage)
    return;

  QScreen * scr = QGuiApplication::primaryScreen();

  double res = scr->physicalDotsPerInch() * 0.8;
  
  QImage image = pdfPage->renderToImage(res, res);

  pageDisplay->setPixmap(QPixmap::fromImage(image));
  pageDisplay->resize(image.size());
}

void DocumentWidget::showDocument(const QString & str)
{
  fileName = str;
  firstLine->setText(tr("<b>Document: </b>%1").arg(fileName));
  document = cabinet->documents.document(fileName);
  attributesEditor->clear();
  if(document) {
    QString dtn = document->docTypeName();
    if(dtn.isEmpty())
      documentTypeCombo->setCurrentText("(none)");
    else {
      documentTypeCombo->setCurrentText(dtn);
    }
    description->setText(document->infoText());
    DocType * dt = document->docType();
    QHash<QString, AttributeHash::HandledType> reqs;
    if(dt)
      reqs = dt->requiredAttributes();
    attributesEditor->editHash(&document->attributes, reqs);
  }
  else {
    documentTypeCombo->setCurrentText("(none)");
    description->setText("");
  }
  showPage(0);
}

void DocumentWidget::onTypeChanged(const QString & newType)
{
  if(! document)
    document = cabinet->documents.modifiableDocument(fileName);
  if(newType == "(none)") {
    document->setDocType(NULL);
  }
  else {
    if(document->docTypeName() == newType)
      return;
    document->setDocType(DocType::namedType(newType));
  }
  showDocument(fileName);
}
