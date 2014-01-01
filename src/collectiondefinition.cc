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

QString CollectionDefinition::getName() const
{
  return name;
}

QString CollectionDefinition::getPublicName() const
{
  return publicName;
}

QString CollectionDefinition::getDescription() const
{
  return description;
}

AttributeHash CollectionDefinition::parseDocumentMetaData(const QString & doctype,
                                                          const AttributeHash & contents)
{
  return code.parseDocumentMetaData(doctype, contents);
}

AttributeHash CollectionDefinition::parseFileMetaData(const QString & doctype,
                                                      const QString & fileName)
{
  return code.parseFileMetaData(doctype, fileName);
}


Fetcher* CollectionDefinition::fetchNewDocuments(const AttributeHash & credentials,
                                                 const QList<AttributeHash> &existingDocuments,
                                                 Collection * target)
{
  return code.fetchNewDocuments(credentials, existingDocuments, target);
}

int CollectionDefinition::scoreForTransaction(Document * doc, AtomicTransaction * tr) const
{
  return code.scoreForTransaction(doc, tr);
}




SerializationAccessor * CollectionDefinition::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  /// \todo This small trick probably could avoid static
  /// "beingSerialized" things, as the serializationAccessor function
  /// is necessarily called for every single time.
  code.definition = this;

  ac->addScalarAttribute("public-name", &publicName);
  ac->addScalarAttribute("description", &description, false);
  ac->addHashAttribute("document-type", &documentTypes);
  ac->addAttribute("ruby-class-code", &code);
  return ac;
}

void CollectionDefinition::dumpContents()
{
  QTextStream o(stdout);
  QHash<QString, DocumentDefinition>::const_iterator i =
    documentTypes.constBegin();
  while (i != documentTypes.constEnd()) {
    o << "Type : " << i.value().name << endl;
    ++i;
  }
  o << "Code (" << code.name << "): " << code.code;
}


/// \todo There should be a way to customize this.
QStringList CollectionDefinition::definitionPath("/home/vincent/Prog/eThunes/xml");

CollectionDefinition * CollectionDefinition::loadWithoutRegistering(const QString &name)
{
  QString fileName(name + ".def.xml");
  for(int i = 0; i < definitionPath.size(); i++) {
    QFile file(definitionPath[i] + "/" + fileName);
    if(file.exists()) {
      file.open(QIODevice::ReadOnly);
      QXmlStreamReader w(&file);
      /// \todo should be factored out ?
      CollectionDefinition * def = new CollectionDefinition;
      while(! w.isStartElement() && ! w.atEnd())
	w.readNext();
      def->readXML(&w);
      if(! def->name.isEmpty() && def->name != name)
	printf("Names should match !\n"); /// \tdexception handle this!
      def->name = name;
      return def;
    }
  }
  return NULL;
}

QHash<QString, CollectionDefinition *> CollectionDefinition::loadedDefinitions;


void CollectionDefinition::loadFromFile(const QString &name)
{
  CollectionDefinition * def = loadWithoutRegistering(name);
  if(def)
    loadedDefinitions[name] = def;
  /// \tdexception raise something if NULL ?
}


QStringList CollectionDefinition::availableDefinitions()
{
  QStringList names;
  for(int i = 0; i < definitionPath.size(); i++) {
    QDir d(definitionPath[i]);
    QStringList files = d.entryList(QStringList("*.def.xml"));
    for(int j = 0; j < files.size(); j++)
      names << files[j].replace(".def.xml", "");
  }
  return names;
}


QHash<QString, QString> CollectionDefinition::documentFileFilters()
{
  QHash<QString, QString> filters;
  QHash<QString, DocumentDefinition>::const_iterator i =
    documentTypes.constBegin();
  while(i != documentTypes.constEnd()) {
    filters[QString("%1 (*.pdf)").arg(i.value().definitionName())] =
      i.key();
    i++;
  }
  return filters;
}

CollectionDefinition * CollectionDefinition::namedDefinition(const QString & name)
{
  if(! loadedDefinitions.contains(name))
    loadFromFile(name);
  return loadedDefinitions.value(name, NULL);
}

