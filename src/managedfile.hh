/** 
    \file managedfile.hh
    How files managed by QMoney should be handled.
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


#ifndef __MANAGEDFILE_HH
#define __MANAGEDFILE_HH

#include <serializable.hh>

class Cabinet;

/// This class holds a file as managed by QMoney.
///
/// \todo Write the whole class, and convert Document to use this for
/// both the real document and the attached files.
class ManagedFile : public Serializable {

  /// The cabinet to which the file belongs, mostly for
  /// Cabinet::baseDirectory().
  Cabinet * cabinet;

  /// The current path of the file. It is absolute.
  QString currentPath;

  QDir baseDirectory() const {return cabinet->baseDirectory(); } ;
public:

  ManagedFile() : cabinet(NULL) {;};

  /// Sets the current path, ie completely forgets anything about the
  /// previous file.
  void newFilePath(const QString & path);

  /// Moves/copies the file to the target path, which is assumed to be
  /// relative to Cabinet::baseDirectory().
  void changeFileName(const QString & newName);

  /// Deletes the file ? Only if managed
  void deleteFile(bool onlyIfManaged = true);

  /// Is the file managed by QMoney, ie is it within
  /// Cabinet::baseDirectory() ?
  bool isFileManaged() const;

  /// Returns a QFileInfo about the managed file
  QFileInfo fileInfo() const;

  /// The current extension of the file. Useful for attachments to a
  /// given Document.
  QString fileExtension() const { return fileInfo().suffix();};

  /// Sets the permissions of the target file ?
  ///
  /// \todo Implement that
  void setPermissions();

  /// Returns the current absolute file path.
  QString filePath() const { return currentPath; };


  /// \todo Ideally, this serializer should handle the conversion from
  /// absolute to relative path on save when isFileManaged.
  virtual SerializationAccessor * serializationAccessor();

  virtual void prepareSerializationRead();


  /// Ensures that the directory in which the given file should be
  /// exists. If the path is absolute, it is used as such; if it is
  /// relative, it is taken as relative to Cabinet::baseDirectory()
  void ensureDirectoryExists(const QString & filePath);

protected:


  /// Moves the managed file into destination (absolute file path).
  void moveFile(const QString &newPath);

  /// Copies the managed file into destination (absolute file path).
  void copyFile(const QString &newPath);

};

#endif
