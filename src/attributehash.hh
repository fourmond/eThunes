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
    Time,
    LastType
  };

  /// Returns the HandledType for the given QVariant
  static HandledType variantType(const QVariant &v);

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
  QString formatString(const QString & format) const;

  /// Formats the given variant according to the given format:
  ///
  /// \li A: v is an integer converted to an amount using
  /// Transaction::formatAmount();
  /// \li M: the month, from 01 to 12
  /// \li y: the year (4-digit number)
  /// \li "date:...": return QDateTime.toString(...)
  static QString formatVariant(QVariant v, const QString &spec);

  /// Takes a format string as accepted by formatString() and returns
  /// a hash of names/types that are requested by this format string.
  static QHash<QString, HandledType> requiredAttributes(const QString & format);

  /// The names of numbered types
  static const char * typeNames[];

  static HandledType namedType(const QString & name);

  /// This object, just like Serializable ones, shouldn't be deleted.
  virtual bool shouldBeDeleted() { return false;};

  /// @name Edition-related static functions
  ///
  /// Here we provide a few functions to create and interact with
  /// widgets able to edit attributes of a given type.
  ///
  /// @{
  
  /// Creates a widget suitable for editing the given type:
  static QWidget * createEditor(HandledType type, QWidget * parent = NULL);

  /// Sets the value of the target editor
  static void setEditorValue(HandledType type, QWidget * editor, 
                             const QVariant &value);

  /// Get the value back from the editor
  static QVariant getEditorValue(HandledType type, QWidget * editor);

  /// Connects an "editor changed" signal to the target's slot.
  static void connectEditorChanged(HandledType type, QWidget * editor,
                                   QObject * target, const char * slot);

  /// @}


  /// @name Interaction with scripts
  ///
  /// @{

  /// Returns a QVariant suitable to represent the attributes in a
  /// script.
  QVariant toScript() const;

  /// Creates a AttributeHash object from a JS value, presumably a
  /// map.
  static AttributeHash fromScript(const QVariant & value);

  /// @}

};
#endif
