/*
    accountpage.cc: Implementation of AccountPage
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
#include <documentlistwidget.hh>
#include <document.hh>
#include <linkshandler.hh>
#include <flowlayout.hh>

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
    QString str = "<table><tr><td>";
    Document * doc = documents[i];
    str += QString("<a href='file://%1'>").arg(doc->filePath()) +
      "<img src='/usr/share/icons/hicolor/32x32/apps/adobe.pdf.png'/></a></td>\n<td> " +
      doc->displayText();
    str += "<br/>" + doc->canonicalFileName();
    for(int k = 1; k < doc->attachmentsNumber(); k++)
      str += QString(" <a href='file://%1'>").arg(doc->filePath(k)) +
        "<img src='/usr/share/icons/hicolor/16x16/apps/adobe.pdf.png'/></a>";
    str += " <a href='attach:" + doc->canonicalFileName() +
      "'>(attach file)</a>"; /// \todo tr around.
    for(int l = 0; l < doc->links.size(); l++)
      if(doc->links[l].linkTarget())
        str += " " +
          LinksHandler::linkTo(doc->links[l].linkTarget(),
                               doc->links[l].linkTarget()->publicTypeName());
    str += "</td></tr></table>";
    QLabel * label = new QLabel(str);
    labels[doc] = label;
    layout->addWidget(label);
  }
}

void DocumentListWidget::clearItems()
{
  QHash<Document *, QLabel *>::iterator i;
  for(i = labels.begin(); i != labels.end(); i++) {
    delete i.value();
  }
}




void DocumentListWidget::setupFrame()
{
  layout = new FlowLayout(this);
}


DocumentListWidget::~DocumentListWidget()
{
}

