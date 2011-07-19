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

LatexTable::Row& LatexTable::Row::operator<<(const QString & str)
{
  elements << str;
  if(str.startsWith("\\multicolumn{")) {
    QRegExp re("multicolumn\\{(\\d+)");
    re.indexIn(str);
    nbCols += re.cap(1).toInt();
  }
  else
    nbCols++;
  return *this;
}

void LatexTable::Row::append(const QStringList &lst)
{
  for(int i = 0; i < lst.size(); i++)
    (*this) << lst[i];
}

void LatexTable::Row::append(const LatexTable::Row &row)
{
  elements += row.elements;
  nbCols += row.nbCols;
}

void LatexTable::Cell::newLine()
{
  rows.append(Row());
}

int LatexTable::Cell::realSize() const
{
  int size = rows.size();
  if(size && rows.last().size() == 0)
    size--;
  return size;
}

void LatexTable::Cell::cleanup()
{
  if(rows.size() && rows.last().size() == 0)
    rows.removeAt(rows.size() - 1);
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
    if(nbRows < cells[i].realSize())
      nbRows = cells[i].realSize();

  Cell c = cells.first();
  c.cleanup();
  c.extendCell(nb, nbRows, def);
  for(int i = 1; i < cells.size(); i++) {
    Cell c2 = cells[i];
    c2.extendCell(nb, nbRows, def);
    for(int j = 0; j < nbRows; j++) {
      c.rows[j].append(padding);
      c.rows[j].append(c2.rows[j]);
    }
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
  if(cells.size())
    cells.last().cleanup();
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

  QStringList lst;

  if(! superHeader.isEmpty())
    lst << QString("\\multicolumn{%1}{c}{%2}\n\\\\\n").
      arg(cs.size()).arg(superHeader);

  if(horizontalCells == 1) {
    if(! noHead)
      lst << header.toString();
    for(int i = 0; i < cells.size(); i++)
      lst << cells[i].toString();
  }
  else {
    QList<Cell> cl;
  
    /// @todo Nothing will work correctly when there are multicolumn
    /// cells, unless LatexTable::Cell::extendCell is modified to take
    /// that into account.
  
    QList<Cell> tmp;
    if(! noHead) {
      for(int i = 0; i < horizontalCells; i++)
        tmp << header;
      cl.append(Cell::joinCells(tmp, nbCols, padding));
    }

    int nbCells = (cells.size() + horizontalCells - 1)/horizontalCells;
    for(int i = 0; i < nbCells; i++) {
      tmp.clear();
      for(int j = 0; j < horizontalCells; j++)
        tmp.append(cells.value(j * nbCells + i, Cell()));
      cl.append(Cell::joinCells(tmp, nbCols, padding));
    }

    for(int i = 0; i < cl.size(); i++)
      lst << cl[i].toString();
  }

  ret += lst.join("\\midrule\n");


  ret += "\\bottomrule\n\\end{tabular}";
  return ret;
}
