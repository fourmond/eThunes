/**
    \file latexoutput.hh
    LatexOutput class: create PDF files using LaTeX as a backend.
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


#ifndef __LATEXOUTPUT_HH
#define __LATEXOUTPUT_HH

/// Use this class just as you would use a QTextStream, with a little
/// syntactic sugar...
class LatexOutput {
  /// Points to the actual document stream (within \begin--\end
  /// document)
  QTextStream * documentStream;

  /// Where the documentStream spits.
  QFile * outFile;

  /// The location of the target
  QString outputPath;

  /// A temporary directory in which all the files will be created
  QDir tempDir;

public:

  /// (almost) the preamble of the document.
  QString preamble;

  /// A list of packages that will be \use'd right before the start of
  /// the document.
  QStringList packages;

  /// A package name-indexed list of options for the packages.
  QHash<QString, QString> packageOptions;


  LatexOutput(const QString & op);

  ~LatexOutput();

  /// To use the LatexOutput stream just like QTextStream.
  template<typename T> LatexOutput & operator<<(const T& t) {
    (*documentStream) << t;
    return *this;
  };

  /// Compiles the file, which produces a PDF file in the current
  /// directory (outputName). The produced PDF file is shown using
  /// desktop services if showOutput is true.
  void compile(bool showOutput = true);

  /// Returns a default preamble.
  ///
  /// @todo This function should probably take arguments.
  static QString makeupPreamble();

  /// Adds a package along with its options.
  void addPackage(const QString & package, const QString & options = "");

};

#endif
