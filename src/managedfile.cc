/*
    managedfile.cc: Implementation of ManagedFile
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

#include <headers.hh>
#include <cabinet.hh>
#include <managedfile.hh>



SerializationAccessor * ManagedFile::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("path", 
		   new SerializationItemScalar<QString>(&currentPath, true));
  return ac;
}


void ManagedFile::prepareSerializationRead()
{
  cabinet = Cabinet::cabinetBeingSerialized;
}

void ManagedFile::newFilePath(const QString & str)
{
  currentPath = str;
}

void ManagedFile::changeFileName(const QString &newPath)
{
  QString path = baseDirectory().absoluteFilePath(newPath);
  if(isFileManaged())
    moveFile(path);
  else
    copyFile(path);
}

bool ManagedFile::isFileManaged() const
{
  return ! baseDirectory().relativeFilePath(currentPath).
    startsWith("..");
}


void ManagedFile::moveFile(const QString &newPath)
{
  ensureDirectoryExists(newPath);
  QFile::rename(currentPath, newPath); 
  /// \tdexception Check the rename went fine
  currentPath = newPath;
}

void ManagedFile::copyFile(const QString &newPath)
{
  ensureDirectoryExists(newPath);
  QFile::copy(currentPath, newPath); 
  /// \tdexception Check the copy went fine
  currentPath = newPath;
}


void ManagedFile::ensureDirectoryExists(const QString & filePath)
{
  QFileInfo i(filePath);
  baseDirectory().mkpath(i.path());
  /// \tdexception Check it went fine !
}
