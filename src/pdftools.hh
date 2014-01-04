/**
    \file pdftools.hh
    Functions to deal with PDF files
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


#ifndef __PDFTOOLS_HH
#define __PDFTOOLS_HH


#include <attributehash.hh>

/// Various PDF tools
namespace PDFTools {
  /// Reads the PDF into an AttributeHash, filling in the contents of
  /// the text, and some meta-data.
  ///
  /// \todo Handle meta-data such as file date/time, file name ? (no
  /// -- why not ? so long as full path isn't included, that shouldn't
  /// leak too much information, especially since most of the time, it
  /// will just be stuff coming from a web server), but file extension
  /// !, and specific PDF information. The former should be done via a
  /// common function (let's think of other kinds of files).
  AttributeHash readPDF(QString file);
};

#endif
