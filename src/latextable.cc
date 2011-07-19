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

void LatexTable::Cell::extendCell(int nbCols, int nbRows, const QString & def)
{
  while(rows.size() < nbRows)
    newLine();

  for(int i = 0; i < rows.size(); i++)
    while(rows[i].size() < nbCols)
      rows[i] << def;
}

LatexTable::Cell LatexTable::Cell::joinCells(const QList<Cell> &cells, int nb,
                                             const QStringList &padding, 
                                             const QString & def)
{
  int nbRows = 0;
  for(int i = 0; i < cells.size(); i++)
    if(nbRows < cells[i].rows.size())
      nbRows = cells[i].rows.size();

  Cell c = cells.first();
  c.extendCell(nb, nbRows, def);
  for(int i = 1; i < cells.size(); i++) {
    Cell c2 = cells[i];
    c2.extendCell(nb, nbRows, def);
    for(int j = 0; j < nbRows; j++)
      c.rows[j] += padding + c2.rows[j];
  }

  return c;
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


QString LatexTable::packTable(int horizontalCells,
                              const QStringList & padding,
                              const QString & padColSpec)
{
  /// @todo implement horizontalCells =! 1

  QStringList cs = colSpecs;
  int nbCols = cs.size();
  for(int i = 1; i < horizontalCells; i++) {
    for(int j = 0; j < padding.size(); j++)
      cs << padColSpec;
    cs += colSpecs;
  }

  QString ret = QString("\\begin{tabular}{%1}\n").
    arg(cs.join(""));

  ret += "\\toprule\n";

  if(horizontalCells == 1) {
    QStringList lst;
    lst << header.toString();
    for(int i = 0; i < cells.size(); i++)
      lst << cells[i].toString();
    ret += lst.join("\\midrule\n");
  }
  else {
    QList<Cell> cl;
  
    /// @todo Nothing will work correctly when there are multicolumn
    /// cells, unless LatexTable::Cell::extendCell is modified to take
    /// that into account.
  
    QList<Cell> tmp;
    for(int i = 0; i < horizontalCells; i++) {
      tmp << header;
    }
    cl.append(Cell::joinCells(tmp, nbCols, padding));

    int nbCells = (cells.size() + horizontalCells - 1)/horizontalCells;
    for(int i = 0; i < nbCells; i++) {
      tmp.clear();
      for(int j = 0; j < horizontalCells; j++)
        tmp.append(cells.value(j * nbCells + i, Cell()));
      cl.append(Cell::joinCells(tmp, nbCols, padding));
    }

    QStringList lst;
    for(int i = 0; i < cl.size(); i++)
      lst << cl[i].toString();
    ret += lst.join("\\midrule\n");
  }



  ret += "\\bottomrule\n\\end{tabular}";
  return ret;
}
