/*
    collection.cc: The Collection class and other related ones
    Copyright 2010, 2014 by Vincent Fourmond

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

#include <ruby-utils.hh>
#include <ruby-templates.hh>

// for readPDF
#include <collectioncode.hh>
#include <fetcher.hh>

VALUE CollectionDefinition::cCollectionDefinition = Qnil;

VALUE CollectionDefinition::getBaseClass()
{
  if(! RTEST(cCollectionDefinition))
    cCollectionDefinition = rb_eval_string("CollectionDefinition");
  return cCollectionDefinition;
}

// The syntax of rb_hash_foreach is kinda obscure, but, well, it looks
// like the function expected is to be called thus:
//
// (arg is the last argument to rb_hash_foreach)
int CollectionDefinition::updateFromRubyHelper(VALUE key, VALUE val, void * arg)
{
  QString k = Ruby::valueToQString(key);
  if(! loadedDefinitions.contains(k)) {
    loadedDefinitions[k] = new CollectionDefinition(val);
  }
  return ST_CONTINUE;
}

VALUE CollectionDefinition::updateFromRubyCodeInternal()
{
  // Creates a CollectionDefinition object from each defined ruby
  // class.
  VALUE hash = rb_iv_get(getBaseClass(), "@collections");
  rb_hash_foreach(hash, (int (*)(...))CollectionDefinition::
                  updateFromRubyHelper, NULL);
  return Qnil;
}

void CollectionDefinition::updateFromRubyCode()
{
  Ruby::run(&CollectionDefinition::updateFromRubyCodeInternal);
}



// The syntax of rb_hash_foreach is kinda obscure, but, well, it looks
// like the function expected is to be called thus:
//
// (arg is the last argument to rb_hash_foreach)
int CollectionDefinition::updateDocumentListHelper2(VALUE key, VALUE val, void * arg)
{
  QString k = Ruby::valueToQString(key);
  CollectionDefinition * def = static_cast<CollectionDefinition *>(arg);
  def->documentTypes[k] = new DocumentDefinition(val);
  return ST_CONTINUE;
}

VALUE CollectionDefinition::updateDocumentListHelper(CollectionDefinition * def)
{
  VALUE hash = rb_iv_get(def->rubyObject, "@documents");
  rb_hash_foreach(hash, (int (*)(...))CollectionDefinition::
                  updateDocumentListHelper2, (VALUE) def);
  return Qnil;
}

void CollectionDefinition::updateDocumentList()
{
  Ruby::run(&CollectionDefinition::updateDocumentListHelper, this);
}

CollectionDefinition::CollectionDefinition(VALUE cls) :
  rubyObject(cls)
{
  updateDocumentList();
  hasFetchMethod = rb_respond_to(cls, Ruby::fetchID);
}

bool CollectionDefinition::canFetch() const
{
  return hasFetchMethod;
}


QString CollectionDefinition::getName() const
{
  VALUE str = Ruby::safeFuncall(rubyObject, Ruby::nameID, 0);
  return Ruby::valueToQString(str);
}

QString CollectionDefinition::getPublicName() const
{
  VALUE str = Ruby::safeFuncall(rubyObject, Ruby::publicNameID, 0);
  return Ruby::valueToQString(str);
}

QString CollectionDefinition::getDescription() const
{
  VALUE str = Ruby::safeFuncall(rubyObject, Ruby::descriptionID, 0);
  return Ruby::valueToQString(str);
}

AttributeHash CollectionDefinition::parseDocumentMetaData(const QString & doctype,
                                                          const AttributeHash & contents)
{
  DocumentDefinition * def = documentDefinition(doctype);
  if(def) {
    return def->parseDocumentMetaData(contents);
  }
  else
    return AttributeHash();
}

AttributeHash CollectionDefinition::parseFileMetaData(const QString & doctype,
                                                      const QString & fileName)
{
  AttributeHash contents = CollectionCode::readPDF(fileName);
  return parseDocumentMetaData(doctype, contents);
}


Fetcher* CollectionDefinition::fetchNewDocuments(const AttributeHash & credentials,
                                                 const QList<AttributeHash> &existingDocuments,
                                                 Collection * target)
{
  Fetcher * n = new Fetcher();
  n->setTarget(target);

  VALUE ary = rb_ary_new();
  for(int i = 0; i < existingDocuments.size(); i++)
    rb_ary_push(ary, existingDocuments[i].toRuby());

  VALUE fiber = 
    Ruby::safeFuncall(rb_eval_string("Net"), Ruby::fetchID, 4, rubyObject, 
                      n->wrapToRuby(),
                      credentials.toRuby(), ary);
  
  n->setFiber(fiber);
  Ruby::safeFuncall(fiber, Ruby::resumeID, 0);
  return n;

}

int CollectionDefinition::scoreForTransaction(Document * doc, AtomicTransaction * tr) const
{
  if(! doc->definition)
    return -1000;

  return doc->definition->scoreForTransaction(doc, tr);
}

void CollectionDefinition::dumpContents()
{
  // QTextStream o(stdout);
  // QHash<QString, DocumentDefinition>::const_iterator i =
  //   documentTypes.constBegin();
  // while (i != documentTypes.constEnd()) {
  //   o << "Type : " << i.value().name << endl;
  //   ++i;
  // }
  // o << "Code (" << code.name << "): " << code.code;
}


/// \todo There should be a way to customize this.
QStringList CollectionDefinition::definitionPath("/home/vincent/Prog/eThunes/collections");

QHash<QString, CollectionDefinition *> CollectionDefinition::loadedDefinitions;


void CollectionDefinition::loadFromFile(const QString &name)
{
  QString fileName(name + ".rb");
  for(int i = 0; i < definitionPath.size(); i++) {
    QString file(definitionPath[i] + "/" + fileName);
    if(QFile::exists(file)) {
      Ruby::safeLoadFile(file);
      break;
    }
  }
  updateFromRubyCode();
}


QStringList CollectionDefinition::availableDefinitions()
{
  QStringList names;
  for(int i = 0; i < definitionPath.size(); i++) {
    QDir d(definitionPath[i]);
    QStringList files = d.entryList(QStringList("*.rb"));
    for(int j = 0; j < files.size(); j++)
      names << files[j].replace(".rb", "");
  }
  return names;
}


QHash<QString, QString> CollectionDefinition::documentFileFilters()
{
  QHash<QString, QString> filters;
  QHash<QString, DocumentDefinition *>::const_iterator i =
    documentTypes.constBegin();
  while(i != documentTypes.constEnd()) {
    filters[QString("%1 (*.pdf)").arg(i.value()->definitionName())] =
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

