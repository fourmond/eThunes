/**
   \file translatable.hh
   Simple wrapper around QObject's tr() function for classes not descending from QObject
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

#ifndef __TRANSLATABLE_HH
#define __TRANSLATABLE_HH

/// All classes that not descending from QObject that harbour
/// translatable strings should descend from this one. This make the
/// work with linguist more easy. It simply wraps QObject's tr.
class Translatable {
protected:

  /// A helper wrapper around QObject's tr
  static QString tr(const char * sourceText, 
             const char * disambiguation = 0, int n = -1) {
    return QObject::tr(sourceText, disambiguation, n);
  };
};

#endif
