/*
    pdftools.cc: tools to read PDF files
    Copyright 2014 by Vincent Fourmond

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
#include <pdftools.hh>


static AttributeHash popplerReadAllPages(Poppler::Document * doc)
{
  AttributeHash val;
  QString layout;
  QString plain;
  if(! doc)
    return val;
  for(int i = 0; i < doc->numPages(); i++) {
    Poppler::Page * page = doc->page(i);
    layout += page->text(QRectF());
    QList<Poppler::TextBox *> boxes = page->textList();
    for(int j = 0; j < boxes.size(); j++) {
      plain += boxes[j]->text();
      if(boxes[j]->hasSpaceAfter())
	plain += " ";
      else
	plain += "\n"; 		// This seems about right, even though
				// it surely won't work on some
				// examples
      delete boxes[j];
    }
    delete page;
  }
  val["text"] = plain;
  val["text-layout"] = layout;
  return val;
}


AttributeHash PDFTools::readPDF(QString file)
{
  AttributeHash retval;
  // Now using Poppler
  Poppler::Document * doc = Poppler::Document::load(file);
  if(doc) {
    retval = popplerReadAllPages(doc);
    delete doc;
  }

  /// \todo here: get some more information ?

  return retval;
}
