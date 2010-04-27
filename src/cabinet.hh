/** 
    \file cabinet.hh
    Class representing a collection of all objects.
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


#ifndef __CABINET_HH
#define __CABINET_HH

#include <serializable.hh>
#include <wallet.hh>
#include <collection.hh>

/// The main container. This class is the main container for all data
/// QMoney (which should change name some time soon)
class Cabinet : public QObject, public Serializable {
  Q_OBJECT;

protected:

  /// Dirty is set when the Cabinet has been modified since the last
  /// time dirty was cleared.
  bool dirty;

public:

  Cabinet();

  /// The one and only Wallet
  Wallet wallet;

  /// A list of Collection objects
  QList<Collection> collections;

  /// Saves the Cabinet into the named file.
  void saveToFile(QString fileName);

  /// Loads a Cabinet from the given file
  void loadFromFile(QString fileName);

  virtual SerializationAccessor * serializationAccessor();

  virtual void prepareSerializationRead();
  
  virtual void finishedSerializationRead();

signals:
  /// Emitted when the dirty flag has changed.
  void dirtyChanged(bool dirty);

public slots:
  /// Sets the dirty flag
  void setDirty(bool dirty = true);

  /// Clears the contents of the Cabinet (such as before loading ;-)...
  void clearContents();

public:
  /// Whether the Cabinet has pending modifications
  bool isDirty() const { return dirty; }; 
};

#endif
