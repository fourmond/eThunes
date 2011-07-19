/*
    latexoutput.cc: utility class to produce PDF files using LaTeX.
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
#include <latexoutput.hh>


LatexOutput::LatexOutput(const QString & op) : 
  outputPath(op)
{

  if(! outputPath.endsWith(".pdf"))
    outputPath += ".pdf";

  /// First, we create the temporary directory
  QDir tmp = QDir::temp();
  QString dir;

  int i = 100;
  while(i > 0) {
    dir = QString("eThunes-PDF-%1").arg(rand());
    if(tmp.mkdir(dir))
      break;
    i--;
  }
  if(! i)
    throw "Couldn't create temporary directory";
  tempDir = tmp;
  if(! tempDir.cd(dir))
    throw "Error: we should have been able to cd into that...";

  preamble = makeupPreamble();

  outFile = new QFile(tempDir.absoluteFilePath("contents.tex"));
  if(!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
    throw "Problem opening file";
  
  documentStream = new QTextStream(outFile);
  documentStream->setCodec("UTF-8");
}

QString LatexOutput::makeupPreamble()
{
  return QObject::tr("\\documentclass{article}\n\n"
                     "\\usepackage[english]{babel}\n"
                     "\\usepackage[utf8]{inputenc}\n"
                     "\\usepackage[T1]{fontenc}\n");
}

LatexOutput::~LatexOutput()
{
  delete documentStream;
  delete outFile;
}

void LatexOutput::compile(bool showOutput)
{
  QFile tmpOut(tempDir.absoluteFilePath("main.tex"));
  if(!tmpOut.open(QIODevice::WriteOnly | QIODevice::Text))
    throw "Problem opening file";

  documentStream->flush();
  
  QTextStream o(&tmpOut);
  o.setCodec("UTF-8");
  o << preamble;
  packages.removeDuplicates();
  for(int i = 0; i < packages.size(); i++) {
    if(packageOptions.contains(packages[i]))
      o << "\\usepackage[" << packageOptions[packages[i]] 
        << "]{" << packages[i] << "}\n";
    else
      o << "\\usepackage{" << packages[i] << "}\n";
  }
  o << preambleAdd;
  o << "\\begin{document}\\input{contents.tex}\\end{document}" << endl;
  tmpOut.close();
  
  QProcess latex;
  QStringList args;
  args << "-interaction=nonstopmode" 
       << "main.tex" ;
  latex.setWorkingDirectory(tempDir.absolutePath());
  latex.closeWriteChannel();
  latex.setProcessChannelMode(QProcess::ForwardedChannels);	
  latex.start("pdflatex", args);
  latex.waitForFinished(-1);

  latex.setWorkingDirectory(tempDir.absolutePath());
  latex.closeWriteChannel();
  latex.setProcessChannelMode(QProcess::ForwardedChannels);	
  latex.start("pdflatex", args);
  latex.waitForFinished(-1);

  // Now, copy back to the path requested...
  QFile::remove(outputPath);    // Hmmmm...
  QFile::copy(tempDir.absoluteFilePath("main.pdf"), outputPath);
  
  QFileInfo info(outputPath);
  if(showOutput && info.exists())
    QDesktopServices::openUrl(QString("file://%1").
                              arg(info.canonicalFilePath()));

}


void LatexOutput::addPackage(const QString & package, 
                             const QString & options)
{
  packages << package;
  if(! options.isEmpty())
    packageOptions[package] = options;
}

void LatexOutput::selectFont(const QString & fontPackage, 
                             const QString & mdfont)
{
  addPackage(fontPackage);
  if(! mdfont.isEmpty())
    addPackage("mathdesign", mdfont);
}


void LatexOutput::makeTitle(const QString & title, 
                            const QString & date,
                            const QString & author)
{
  preamble += QString("\\title{%1}\n\\author{%2}\n\\date{%3}\n").
    arg(title).
    arg(author).
    arg(date);
  preamble += "\\AtBeginDocument{\\maketitle}\n";
}
