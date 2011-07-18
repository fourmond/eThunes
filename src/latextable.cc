/*
  latextable.cc: implementation of the LatexTable class
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
#include <latextable.hh>

void LatexTable::Cell::newLine()
{
  rows.append(QStringList());
}

LatexTable::Cell& LatexTable::Cell::operator<<(const QString & str)
{
  if(! rows.size())
    newLine();
  rows.last() << str;
  return *this;
}

QString LatexTable::Cell::toString() const
{
  QString ret;
  for(int i = 0; i < rows.size(); i++) {
    if(rows[i].size() > 0)
      ret += QString("%1 \\\\\n").
        arg(rows[i].join(" & "));
  }
  return ret;
}

void LatexTable::addHeader(const QString & h, const QString & headSpec,
                           int nbCols, const QString &colSpec,
                           const QString & format)
{
  QString fmt = format;
  if(format.isEmpty())
    fmt = headerFormat;
  header << QString("\\multicolumn{%1}{%2}{%3{}%4}").
    arg(nbCols).arg(headSpec).arg(fmt).arg(h);
  QString cs = colSpec;
  
  if(cs.isEmpty())
    cs = defaultSpec;
  for(int i = 0; i < nbCols; i++)
    colSpecs << cs;
}

void LatexTable::newCell()
{
  cells.append(Cell());
}

void LatexTable::ensureHasCells()
{
  if(! cells.size())
    newCell();
}

void LatexTable::newLine()
{
  ensureHasCells();
  cells.last().newLine();
}

LatexTable & LatexTable::operator<<(const QString &str) 
{
  ensureHasCells();
  cells.last() << str;
  return *this;
}


QString LatexTable::packTable(int horizontalCells)
{
  /// @todo implement horizontalCells =! 1

  QString ret = QString("\\begin{tabular}{%1}\n").
    arg(colSpecs.join(""));

  ret += "\\toprule\n";

  QStringList lst;
  lst << header.toString();
  for(int i = 0; i < cells.size(); i++)
    lst << cells[i].toString();
  ret += lst.join("\\midrule\n");

  ret += "\\bottomrule\n\\end{tabular}";
  return ret;
}
