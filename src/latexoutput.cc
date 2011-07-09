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


LatexOutput::LatexOutput(const QString & on) : 
  outputName(on)
{
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
  
}
