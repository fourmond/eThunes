/**
   \file latextable.hh
   LatexTable class: create nice LaTeX tables
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


#ifndef __LATEXTABLE_HH
#define __LATEXTABLE_HH

/// A class to create nice LaTeX tables with relatively little pain.
/// 
/// @todo Obviously, one of the things that may come in really useful
/// would be a class to make tables, providing:
/// \li ways to handle headers (including multicolumns)
/// \li automatic lines
/// \li possibility to splice/wrap (as I did for the monthly summary
/// in AMEmployer)
/// \li warnings when applicable
class LatexTable {

  /// A single row
  class Row {
    int nbCols;
  public:

    QStringList elements;

    Row& operator<<(const QString &str);

    /// The current size of the row
    int size() const { return nbCols;} ;

    Row() : nbCols(0) {;};

    QString join(const QString & sep = "") const {
      return elements.join(sep);
    };

    /// Appends the given elements
    void append(const QStringList &lst);

    /// Appends the given row
    void append(const Row & r);
  };
  
  /// A cell, ie a set of lines/columns that can't be divided. Most of
  /// the times, it will be the whole insides of the table. Cells are
  /// joined by midrules. In some cases, several Cell objects may be
  /// joined together horizontally.
  class Cell {
  public:
    QList<Row> rows;

    Cell & operator<<(const QString &);

    void newLine();

    /// Turns the cell into a suitabe string representation.
    QString toString() const;

    /// Extend the Cell so that it has at least that many rows and
    /// columns.
    void extendCell(int nbCols, int nbRows, const QString & def);

    /// Join different cells in a horizontal fashion.
    ///
    /// padding are the elements that get inserted between each cell,
    /// and def in all the elements that are empty.
    static Cell joinCells(const QList<Cell> &cells, int nb, 
                          const QStringList &padding, 
                          const QString & def = "");

    /// Returns the number of rows, minus the last one if it is empty.
    int realSize() const;

    /// remove the last Row if empty
    void cleanup();
  };

  /// The inner list of cells.
  QList<Cell> cells;

  /// A header cell.
  Cell header;

  /// A superheader line that will cap all cells joined together
  QString superHeader;

  /// The default header spec.
  QString defaultSpec;

  /// The default header format
  QString headerFormat;

  /// The specifications of the columns
  QStringList colSpecs;

  /// Ensure there is at least one cell.
  void ensureHasCells();


public:

  void setSuperHeader(const QString &str) {
    superHeader = str;
  };

  bool noHead; 

  LatexTable(const QString &def = "r", 
             const QString & format = "") : 
    defaultSpec(def), 
    headerFormat(format), noHead(false) {;};

  /// Add a string to the given cell. A warning will be issued if
  /// there are more strings in a line than headers.
  LatexTable & operator<<(const QString & str);

  template<typename T> LatexTable & operator<<(T t) {
    return (*this) << QString("%1").arg(t);
  };

  /// Adds the given header.
  void addHeader(const QString & header, const QString & headSpec = "c",
                 int nbCols = 1, const QString &colSpec = "",
                 const QString & format = QString());

  /// Adds the given headers.
  void addHeader(const QStringList & headers, const QString & headSpec = "c",
                 int nbCols = 1, const QString &colSpec = "",
                 const QString & format = QString()) {
    for(int i = 0; i < headers.size(); i++)
      addHeader(headers[i], headSpec, nbCols, colSpec, format);
  };
  
  /// Starts a new line within the current cell
  void newLine();

  /// Starts a new cell
  void newCell();

  /// Packs the table and returns the corresponding string
  QString packTable(int horizontalCells = 1, 
                    const QStringList & padding = QStringList(),
                    const QString & padColSpec = "c");

  QString packTable(int horizontalCells, 
                    const QString & padding,
                    const QString & padColSpec = "c") {
    QStringList pad;
    pad << padding;
    return packTable(horizontalCells, pad, padColSpec);
  };

};

#endif
