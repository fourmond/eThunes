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
#include <documentlist.hh>

class Plugin;

/// The main container. This class is the main container for all data
/// eThunes (which should change name some time soon)
class Cabinet : public QObject, public Serializable {
  Q_OBJECT;

protected:

  /// Dirty is set when the Cabinet has been modified since the last
  /// time dirty was cleared.
  bool dirty;

  /// The file name
  QString filePath;


  /// Pointer to the unique Cabinet object ?
  static Cabinet * theCabinet;
public:


  Cabinet();
  ~Cabinet();

  /// The one and only Wallet
  Wallet wallet;

  /// The list of Documents
  DocumentList documents;

  /// All plugins. Please note these are stored as pointers, not
  /// objects...
  ///
  /// @todo Turn that into a watchable list some day...
  QList<Plugin*> plugins;

  /// Saves the Cabinet into the named file.
  ///
  /// \todo Maybe change into a saveTo/save stuff ? Since the file
  /// name shouldn't change too much in the end.
  void saveToFile(QString filePath);

  /// Loads a Cabinet from the given file
  void loadFromFile(const QString & filePath);

  virtual SerializationAccessor * serializationAccessor();

  virtual void prepareSerializationRead();

  virtual void finishedSerializationRead();

  /// The full file name of the Cabinet.
  QString fullFilePath() const {
    return filePath;
  };

  /// The file name of the Cabinet
  QString fileName() const {
    return QFileInfo(filePath).fileName();
  };


  /// The base directory for file storage. All path returned by
  /// Document::canonicalFileName should be relative to this
  /// directory.
  QDir baseDirectory();


signals:

  /// Emitted when the dirty flag has changed.
  void dirtyChanged(bool dirty);

  /// Emitted whenever the file name has changed.
  ///
  /// \todo handle changing directory !
  void filenameChanged(const QString & filePath);

public slots:
  /// Sets the dirty flag
  void setDirty(bool dirty = true);

  /// Clears the contents of the Cabinet (such as before loading ;-)...
  void clearContents();

public:
  /// Whether the Cabinet has pending modifications
  bool isDirty() const { return dirty; };

  /// The Cabinet currently serialized.
  static Cabinet * cabinetBeingSerialized;

  /// Returns the only instance of the Cabinet.
  static Cabinet * globalCabinet();

};

#endif
