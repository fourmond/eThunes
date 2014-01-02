/*
    collection.cc: The Collection class and other related ones
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
#include <collection.hh>
#include <cabinet.hh>

/// A small helper class to handle the serialization of a
/// CollectionDefinition pointer.
class SerializeCollectionDefinitionPointer : public SerializationItem {
  CollectionDefinition **target;
public:
  SerializeCollectionDefinitionPointer(CollectionDefinition **t) { target = t;};

  virtual void setFromVariant(const QVariant &v) {
    setFromString(v.toString());
  };

  virtual void setFromString(const QString &str) {
    /// \tdexception Probably check for NULL ?
    *target = CollectionDefinition::namedDefinition(str);
  };


  virtual QString valueToString() {
    if(*target) {
      return (*target)->getName();
    }
    /// \tdexception Probably this should never occur.
    return QString();
  };

  virtual QVariant valueToVariant() {
    return QVariant(valueToString());
  };

};


//////////////////////////////////////////////////////////////////////


QString Collection::documentFileNameFormat(const DocumentDefinition * def) 
  const {
  /// @todo Here, we should implement user overrides.
  QString base;
  if(! baseDirectory.isEmpty())
    base = baseDirectory + "/";
  return base + def->fileNameFormat;
};



SerializationAccessor * Collection::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("definition",
		   new SerializeCollectionDefinitionPointer(&definition));
  ac->addScalarAttribute("name", &name);
  // Now, we try the list stuff...
  ac->addListAttribute("document", &documents);
  return ac;
}

Document * Collection::importFile(const QString & doctype,
				  const QString & file)
{
  Document doc;
  /// \tdexception handle the exceptions raised ? (or do that in the
  /// calling functions ?)
  AttributeHash attrs = definition->parseFileMetaData(doctype, file);
  // Already have one !
  if(fileClashes(definition->documentTypes[doctype], attrs))
    return NULL;

  DocumentDefinition * def = definition->documentTypes[doctype];

  /// @todo Somehow, prompting here isn't that great. A way around
  /// this could be to define a collection of prompting facilities
  /// (query for string, query for yes/no, warning, etc ?) as an
  /// abstract class and pass appropriate children of that around ?
  /// (but that's clearly a pain ;-)...)
  {

    /// @todo Now, I must add an attribute to either the
    /// DocumentDefinition or the CollectionDefinition asking whether
    /// it is OK or not to have missing attributes (if OK, then
    /// prompt). Probably at the level of the CollectionDefinition ?
    QStringList missingAttributes = 
      missingAttributesForDocument(attrs, def);

    if(missingAttributes.size() > 0) {
      QString msg = QObject::tr("The document %1 could not be imported "
                                "because the following attributes are "
                                "missing: %2.").
        arg(file).arg(missingAttributes.join(", "));
      QMessageBox::warning(NULL, QObject::tr("Missing attributes for %1").
                           arg(file),
                           msg);
      return NULL;
    }
  }
  

  doc.attributes = attrs;
  doc.definition = def;
  doc.collection = this;
  doc.setFilePath(file);
  doc.bringFileIntoOwnership();
  documents.append(doc);

  cabinet->registerDocument(&documents.last());
  return &documents.last();
}

QHash<DocumentDefinition *, QList<Document *> > Collection::typeDocuments()
{
  QHash<DocumentDefinition *, QList<Document *> > dd;
  for(int i = 0; i < documents.size(); i++) {
    Document * doc = &documents[i];
    dd[doc->definition] << doc;
  }
  return dd;
}

QList<Document *> Collection::allDocuments()
{
  QList<Document *> list;
  for(int i = 0; i < documents.size(); i++)
    list << &documents[i];
  return list;
}

Collection * Collection::collectionBeingSerialized = NULL;

void Collection::prepareSerializationRead()
{
  collectionBeingSerialized = this;
  cabinet = Cabinet::cabinetBeingSerialized;
}

void Collection::finishedSerializationRead()
{
  collectionBeingSerialized = NULL;
}

/// \todo This is very ineffective: a hash should be used eventually,
/// either as a QString -> Document * cache or as the primary data
/// storage.
bool Collection::fileClashes(const QString & cn) const
{
  if(cabinet->namedDocument(cn))
    return true;
  return false;
}

void Collection::fetchNewDocumentsForUser(const AttributeHash &user)
{
  QList<AttributeHash> docs;
  for(int i = 0; i < documents.size(); i++)
    docs << documents[i].documentAttributes();
  definition->fetchNewDocuments(user, docs, this);
}

QHash<QString, AttributeHash::HandledType> Collection::attributesRequiredForDocument(const DocumentDefinition * def) const
{
  typedef QHash<QString, AttributeHash::HandledType> Hash;
  Hash ret = AttributeHash::requiredAttributes(documentFileNameFormat(def));
  Hash t2 = AttributeHash::requiredAttributes(documentDisplayFormat(def));

  // Here, we reimplement QHash::unite, but without the troubles with
  // a key used multiple times...
  for(Hash::iterator i = t2.begin(); i != t2.end(); i++)
    if(! ret.contains(i.key()))
      ret[i.key()] = i.value();
  return ret;
}

QStringList Collection::missingAttributesForDocument(const AttributeHash & attrs, 
                                                     const DocumentDefinition * def) const
{
  QStringList missingAttributes;
  typedef QHash<QString, AttributeHash::HandledType> Hash;
  Hash req = attributesRequiredForDocument(def);
  for(Hash::iterator i = req.begin(); i != req.end(); i++)
    if(! attrs.contains(i.key()))
      missingAttributes << i.key();
  return missingAttributes;
}
