/**
    \file serializable.hh
    Infrastructure for data serialization (XML, Settings...)
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

#ifndef __SERIALIZABLE_HH
#define __SERIALIZABLE_HH
#include <serialization.hh>
#include <watchable.hh>

class XmlReader;

/// All classes that should be serialized at some point should include
/// this class in their ancestry (but not necessarily as first
/// superclass). It shouldn't contain data.
///
/// All children must reimplement serializationAccessor() to return
/// the proper SerializationAccessor to handle the serialization.
///
/// It is a child of SerializationAttribute in order to make it
/// dreadfull easy to embed complex objects in attributes ;-)...
class Serializable : public SerializationAttribute,
                     public virtual Watchable {


public:

  Serializable();

  /// This function should be reimplemented in children, as it
  /// provides the way for the object to be serialized.
  ///
  /// A subclass will return an appropriate SerializationAccessor
  /// object to set/get its data using a unique interface.
  virtual SerializationAccessor * serializationAccessor() = 0;

  /// \name Serialization hooks
  ///
  /// Functions called at some time in the reading or writing process,
  /// for instance to ensure the attributes are in a well-defined
  /// state.
  ///
  /// @todo Find a way to add hooks to the serializationAccessor
  /// stuff... Possibly in a list fashion ? (pushing
  /// functions-to-member ?)
  ///
  /// @{

  /// This hook is called before the loading, to prepare it (and in
  /// particular, to clean attributes that should be empty)
  virtual void prepareSerializationRead() {;};

  /// This hook is called after the loading, to ensure all "derived
  /// attributes" are setup properly.
  virtual void finishedSerializationRead() {;};

  /// This hook is called before the writing, to prepare it
  virtual void prepareSerializationWrite() {;};

  /// This hook is called after the writing.
  virtual void finishedSerializationWrite() {;};

  /// @}

  /// \name Saving/loading functions
  ///
  /// Functions to save/load Serializable objects from various
  /// sources.
  /// @{

  /// Writes the Serializable object into the writer as name.
  virtual void writeXML(const QString & name, QXmlStreamWriter * writer);


  /// Reads the Serializable object from the reader.
  virtual void readXML(XmlReader * reader);

  /// @}

  /// Serializable object definitely should *not* be free by
  /// default...
  virtual bool shouldBeDeleted() { return false;};

  virtual ~Serializable();
};

#endif
