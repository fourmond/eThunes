/** 
    \file ofximport.hh
    A simple way to import OFX 'downloads' from banks.
    Copyright 2008 by Vincent Fourmond

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


#ifndef __OFXIMPORT_HH
#define __OFXIMPORT_HH

#include <account.hh>

/// An import from an OFX 'download'
///
/// \todo This class should parse the balance information, very
/// important to check that the things are done properly.
///
/// \todo Maybe this class should have an ancestor that would be
/// common to all the ways to import banking information, although for
/// now I'll stick to OFX.
///
/// \todo This class should have a "source" member to trace from where
/// it is coming (file name, URL...)
class OFXImport {
public:
  QList<Transaction> transactions;
  QList<Account> accounts;

  /// Import the contents of a OFX file as an OFXImport.
  static OFXImport importFromFile(QString file);

  /// Import the contents of a OFX file as an OFXImport.
  static OFXImport importFromFile(QIODevice * stream);

};


#endif
