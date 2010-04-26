/** 
    \file attributehash.hh
    The attribute hash 
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


#ifndef __ATTRIBUTEHASH_HH
#define __ATTRIBUTEHASH_HH

#include <serializable.hh>

/// A hash of attributes.
///
/// This class hold a hash of attributes, ie data that can be
/// associated with Document objects (for instance). It handles
/// several types:
/// \li strings
/// \li integers
/// \li dates (full dates, ie QDateTime)
///
/// It provides a conversion mechanism to/from Ruby values, through
/// the functions : ...
///
/// Most importantly, it also provides a formatting mechanism based on
/// the contents of the hash; this is central to any string pertaining
/// to the Document system.
///
/// For serialization: reimplement the writeXML/readXML of
/// SerializationAttribute
class AttributeHash : public QHash<QString, QVariant>, 
		      public SerializationAttribute {
public:

  enum HandledType {
    String,
    Number,
    Time
  };

  /// Returns the HandledType for the given QVariant
  static HandledType variantType(const QVariant &v);

  /// Conversion to a Ruby hash
  ///
  /// \warning This function assumes that Ruby has been initialized
  /// before !
  VALUE toRuby() const;

  /// Loading from a Ruby hash; clears initial content if clear is
  /// true.
  ///
  /// \warning This function assumes that Ruby has been initialized
  /// before !
  void setFromRuby(VALUE hash, bool clear);

  /// Same as setFromRuby(VALUE, bool), with the second argument set
  /// to true.
  void setFromRuby(VALUE hash) { setFromRuby(hash, true);};

  /// Converts a QVariant to Ruby VALUE, keeping information
  ///
  /// \warning This function assumes that Ruby has been initialized
  /// before !
  static VALUE variantToRuby(const QVariant & variant);

  /// Converts a Ruby VALUE to QVariant
  ///
  /// \warning This function assumes that Ruby has been initialized
  /// before !
  static QVariant rubyToVariant(VALUE value);

  /// Dump the contents of the hash, for debugging purposes
  void dumpContents() const;

  // serialization stuff
  virtual void writeXML(const QString& name, QXmlStreamWriter* writer);
  virtual void readXML(QXmlStreamReader* reader);


  /// Returns a string based on format within which the constructs
  /// %{key} are replaced by the corresponding string values (or undef
  /// if there is no such key);
  ///
  /// Optionnally %{key%spec} uses a certain format conversion ; see
  /// the documentation of formatVariant for more documentation.
  /// 
  /// \warning Keys that contain % will not be interpreted correctly !
  QString formatString(const QString & format);

  /// Formats the given variant according to the given format:
  /// 
  /// \li A: v is an integer converted to an amount using
  /// Transaction::formatAmount();
  /// \li M: the month, from 01 to 12
  /// \li y: the year (4-digit number)
  /// \li "date:...": return QDateTime.toString(...)
  static QString formatVariant(QVariant v, const QString &spec);

  /// The names of numbered types
  static const char * typeNames[];

  static HandledType namedType(const QString & name);

  /// This object, just like Serializable ones, shouldn't be deleted.
  virtual bool shouldBeDeleted() { return false;};

};

/// and the class to serialize it ?

#endif
