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
  ac->addAttribute("attributes", &attributes);
  ac->addAttribute("definition", 
		   new SerializeDocumentDefinitionPointer(&definition));
  
  ac->addAttribute("attachement",
		   new SerializationQList<ManagedFile>(&attachedFiles));
  return ac;
}



QString Document::displayText() const
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
  if(! attachedFiles.size())
    attachedFiles.append(ManagedFile(collection->cabinet, newPath));
  else
    attachedFiles[0].newFilePath(newPath);
}

void Document::attachAuxiliaryFile(const QString & newPath)
{
  QFileInfo i(newPath);
  if(! i.exists()) {
    /// \tdexception Be unhappy about missing files.
    fprintf(stderr, "Missing file !\n");
    return;
  }
  attachedFiles << ManagedFile(collection->cabinet, newPath);
}

QString Document::canonicalFileName(int i) const
{
  if(i) {
    QString str = attributes.formatString(collection->
					  documentFileNameFormat(this));
    str.insert(str.lastIndexOf("."),QString("-%1").arg(i));
    return str;
  }
  else
    return attributes.formatString(collection->documentFileNameFormat(this));
}

bool Document::isFileCanonical(int file) const
{
  return attachedFiles[file].relativeFilePath() == 
    canonicalFileName(file);
}

QString Document::canonicalFilePath(int i) const
{
  QDir basedir = collection->cabinet->baseDirectory();
  return basedir.absoluteFilePath(canonicalFileName(i));
}


void Document::bringFileIntoOwnership(int file)
{
  QString cf = canonicalFileName(file);
  if(! (attachedFiles[file].relativeFilePath() == cf))
    attachedFiles[file].changeFileName(cf);
}
