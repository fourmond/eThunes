/** 
    \file rubymodulecode.hh
    Code underlying a Collection (Ruby module variant)
    Copyright 2010 by Vincent Fourmond

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


#ifndef __RUBYMODULECODE_HH
#define __RUBYMODULECODE_HH

#include <serializable.hh>
#include <collectioncode.hh>

/// In this way to see the code, functions are reachable using module
/// functions (or rather "class functions" ?)
/// 
/// \li the function to parse and extract meta-data from document type
/// "type" is named parse_type, takes ... and returns ... (- should
/// automatically be converted to _)
class RubyModuleCode : public CollectionCode {
public:
  
  /// Lower level ;-)... Bound to disappear shortly.
  virtual void parseDocumentMetaData(QString doctype, QString fileName);

  virtual SerializationAccessor * serializationAccessor();

  RubyModuleCode();

  QString name;

  QString code;

  /// Loads the module if necessary. Runs arbitrary Ruby code for now !
  ///
  /// \todo Find a way to decrease the number of problems with time !
  void loadModule();

protected:
  /// Whether Ruby has been initialized properly
  static bool hasRubyStarted;

  /// Inits Ruby if necessary
  void initRuby();


  
  bool moduleLoaded;
};

#endif
