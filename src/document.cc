/*
    document.cc: The Document and DocumentDefinition classes
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
#include <document.hh>
#include <collection.hh>
#include <cabinet.hh>

SerializationAccessor * DocumentDefinition::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("name", 
		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("display-format",
		   new SerializationItemScalar<QString>(&displayFormat));
  ac->addAttribute("filename-format",
		   new SerializationItemScalar<QString>(&fileNameFormat));

  ac->addAttribute("public-name",
		   new SerializationItemScalar<QString>(&publicName));
  ac->addAttribute("description",
		   new SerializationItemScalar<QString>(&description));

  return ac;
}

/// A small helper class to handle the serialization of a
/// DocumentDefinition pointer.
class SerializeDocumentDefinitionPointer : public SerializationItem {
  DocumentDefinition **target;
public:
  SerializeDocumentDefinitionPointer(DocumentDefinition **t) { target = t;};

  virtual void setFromVariant(const QVariant &v) {
    setFromString(v.toString());
  };
  
  virtual void setFromString(const QString &str) {
    /// \tdexception Probably check for NULL ?
    *target = Collection::collectionBeingSerialized->definition->
		documentDefinition(str);
  };


  virtual QString valueToString() {
    if(*target) {
      return (*target)->name;
    }
    /// \tdexception Probably this should never occur.
    return QString();
  };

  virtual QVariant valueToVariant() {
    return QVariant(valueToString());
  };
  
};



SerializationAccessor * Document::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("file", 
		   new SerializationItemScalar<QString>(&currentFilePath, 
							true));
  ac->addAttribute("attributes", &attributes);
  ac->addAttribute("definition", 
		   new SerializeDocumentDefinitionPointer(&definition));
  return ac;
}



QString Document::displayText()
{
  return attributes.formatString(collection->documentDisplayFormat(this));
}


void Document::prepareSerializationRead()
{
  collection = Collection::collectionBeingSerialized;
}

void Document::setFilePath(const QString & newPath)
{
  QFileInfo i(newPath);
  if(! i.exists()) {
    /// \tdexception Be unhappy about missing files.
    fprintf(stderr, "Missing file !\n");
    return;
  }
  currentFilePath = i.canonicalFilePath();
}

bool Document::isFileExternal()
{
  QDir basedir = collection->cabinet->baseDirectory();
  return 
    basedir.relativeFilePath(currentFilePath).startsWith("..");
}

QString Document::canonicalFileName() const
{
  return attributes.formatString(collection->documentFileNameFormat(this));
}

bool Document::isFileCanonical()
{
  QDir basedir = collection->cabinet->baseDirectory();
  return 
    basedir.relativeFilePath(currentFilePath) == canonicalFileName();  
}

QString Document::canonicalFilePath() const
{
  QDir basedir = collection->cabinet->baseDirectory();
  return basedir.absoluteFilePath(canonicalFileName());
}

/// Ensures the directory of the given path exists, creating it if
/// that isn't the case.
///
/// \todo Maybe this function should be turned into a util something
static void ensureDirectoryExists(const QString & filePath)
{
  QFileInfo i(filePath);
  /// \todo This won't work on windows.
  QDir("/").mkpath(i.absolutePath());
  /// \tdexception Check it went fine !
}

void Document::copyFileToCanonical()
{
  QString target = canonicalFilePath();
  ensureDirectoryExists(target);
  QFile::copy(currentFilePath, target); 
  /// \tdexception Check the copy went fine
  currentFilePath = target;
}

void Document::moveFileToCanonical()
{
  QString target = canonicalFilePath();
  ensureDirectoryExists(target);
  QFile::rename(currentFilePath, target); 
  /// \tdexception Check the rename went fine
  currentFilePath = target;
}

void Document::bringFileIntoOwnership()
{
  if(isFileExternal()) 
    copyFileToCanonical();
  else
    moveFileToCanonical();
}
