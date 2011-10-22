/*
    documentlistwidget.cc: Implementation of DocumentWidget and DocumentListWidget
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

#include <headers.hh>
#include <documentlistwidget.hh>
#include <document.hh>
#include <flowinggridlayout.hh>
#include <htlabel.hh>

#include <httarget-templates.hh>

DocumentWidget::DocumentWidget(Document * doc) : 
  document(doc)
{
  QHBoxLayout * layout = new QHBoxLayout(this);
  contents = new HTLabel();
  layout->addWidget(contents);
  setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  setFocusPolicy(Qt::ClickFocus);
  updateContents();
  /// @todo automatically update contents when applicable.

  setContentsMargins(QMargins(3,3,3,3));
}

void DocumentWidget::updateContents()
{
  QString str = "<table><tr><td>";
  /// @todo Find a way to customize icons (and icon size) or to get
  /// them from the desktop theme.
  str += QString("<a href='file://%1'>").arg(document->filePath()) +
    "<img src='/usr/share/icons/hicolor/24x24/apps/adobe.pdf.png'/></a></td>\n<td> " +
    document->displayText();
  str += "<br/>" + document->canonicalFileName();

  for(int k = 1; k < document->attachmentsNumber(); k++)
    str += QString(" <a href='file://%1'>").arg(document->filePath(k)) +
      "<img src='/usr/share/icons/hicolor/16x16/apps/adobe.pdf.png'/></a>";
  str += " " + HTTarget::linkToMember(tr("(attach file)"), this,
                                      &DocumentWidget::promptForFileAttachment);

  if(document->links.size() > 0)
    str += " " + document->links.htmlLinkList().join(", ");
  str += "</td></tr></table>";
  contents->setText(str);
}


void DocumentWidget::promptForFileAttachment()
{
  QString file =
    QFileDialog::getOpenFileName(this,
				 tr("Attach a file to %1").
                                 arg(document->canonicalFileName()));
  if(file.isEmpty())
    return;
  document->attachAuxiliaryFile(file);
  document->bringFileIntoOwnership(-1);
  updateContents();             /// @todo update shouldn't be done
                                /// manually.
}

void DocumentWidget::focusInEvent(QFocusEvent * event)
{
  // QTextStream o(stdout);
  // o << "Got focusIn for document: " << document 
  //   << document->canonicalFileName() << endl;
  emit(documentSelected(document));
  QWidget::focusInEvent(event);
}


//////////////////////////////////////////////////////////////////////

DocumentListWidget::DocumentListWidget(const QList<Document*> & documents,
                                       QWidget * parent) :
  QWidget(parent) {
  setupFrame();
  showDocuments(documents);
}

DocumentListWidget::DocumentListWidget(QWidget * parent) :
  QWidget(parent)  {
  setupFrame();
}

void DocumentListWidget::showDocuments(const QList<Document*> & documents)
{
  // First, delete all remaining items:
  clearItems();
  for(int i = 0; i < documents.size(); i++) {
    DocumentWidget * dw = new DocumentWidget(documents[i]);
    widgets[documents[i]] = dw;
    layout->addWidget(dw);
  }
}

void DocumentListWidget::clearItems()
{
  QHash<Document *, DocumentWidget *>::iterator i;
  for(i = widgets.begin(); i != widgets.end(); i++) {
    delete i.value();
  }
}




void DocumentListWidget::setupFrame()
{
  layout = new FlowingGridLayout(this);
}




DocumentListWidget::~DocumentListWidget()
{
}

