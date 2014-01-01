/*
    cabinet.cc: The Cabinet class
    Copyright 2010, 2011 by Vincent Fourmond

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
#include <plugin.hh>
#include <serializable-pointers.hh>


Cabinet::Cabinet() : dirty(false)
{
  /// @todo Use watchChild rather, and 
  watchChild(&wallet, "wallet");
  connect(*this, SIGNAL(changed(const Watchdog *)), SLOT(setDirty()));
  connect(this, SIGNAL(collectionsPossiblyChanged()), SLOT(setDirty()));
  connect(this, SIGNAL(documentsChanged(Collection *)), SLOT(setDirty()));
}


SerializationAccessor * Cabinet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("wallet", &wallet);
  ac->addListAttribute("collection", &collections);
  ac->addAttribute("plugin",
		   new SerializationPointerQList<Plugin>(&plugins));
  return ac;
}

void Cabinet::saveToFile(QString name)
{
  QFile file(name);
  file.open(QIODevice::WriteOnly);
  QXmlStreamWriter w(&file);
  w.setAutoFormatting(true);
  w.setAutoFormattingIndent(2);
  w.writeStartDocument();
  writeXML("cabinet", &w);
  w.writeEndDocument();
  setDirty(false);
  if(name != filePath) {
    filePath = name;
    emit(filenameChanged(filePath));
  }
}


void Cabinet::loadFromFile(QString name)
{
  QFile file(name);
  QTextStream o(stdout);
  file.open(QIODevice::ReadOnly);
  QXmlStreamReader w(&file);
  while(! w.isStartElement() && ! w.atEnd())
    w.readNext();
  readXML(&w);
  filePath = name;
  emit(filenameChanged(filePath));
  emit(collectionsPossiblyChanged());
  setDirty(false);
}


void Cabinet::clearContents()
{
  wallet.clearContents();
  collections.clear();
  /// \todo emit signals here ?
}

Cabinet * Cabinet::cabinetBeingSerialized = NULL;


void Cabinet::prepareSerializationRead()
{
  clearContents();
  cabinetBeingSerialized = this;
}

void Cabinet::finishedSerializationRead()
{
  cabinetBeingSerialized = NULL;
  rebuildDocumentsHash();
  /// \todo Idem for transactions one day ?
  Link::finalizePendingLinks(this);

  // Set the cabinet linkback to the plugins
  for(int i = 0; i < plugins.size(); i++)
    plugins[i]->cabinet = this;
}


void Cabinet::setDirty(bool d)
{
  if(d == dirty)
    return;

  dirty = d;
  emit(dirtyChanged(dirty));
}

QDir Cabinet::baseDirectory()
{
  QFileInfo f(filePath);
  return QDir(f.canonicalPath());
}

Collection * Cabinet::addNewCollection(const QString &name,
				       CollectionDefinition * def)
{
  collections.append(Collection());
  Collection * c = &collections.last();
  c->name = name;
  c->definition = def;
  c->cabinet = this;
  /// \todo emit something
  /// \tdexception Raise on NULL CollectionDefinition.
  return c;
}

void Cabinet::registerDocument(Document * doc, bool signal)
{
  QString name = doc->canonicalFileName();
  if(namedDocument(name)) {
    fprintf(stderr, "We have a document clash !\n");
    /// \tdexception do something here on documents clash
  }
  documentsByName[name] = doc;
  if(signal) {
    emit(documentsChanged(doc->collection));
  }
}

void Cabinet::rebuildDocumentsHash()
{
  documentsByName.clear();
  for(int i = 0; i < collections.size(); i++)
    for(int j = 0; j < collections[i].documents.size(); j++)
      registerDocument(&collections[i].documents[j], false);
}

QList<Document *> Cabinet::allDocuments()
{
  QList<Document *> retval;
  for(int i = 0; i < collections.size(); i++)
    retval += collections[i].allDocuments();
  return retval;
}

TransactionPtrList Cabinet::transactionMatchingCandidates(Document * document)
{
  DocumentDefinition * def = document->definition;
  if(def->relevantDate.isEmpty() ||
     ! document->attributes.contains(def->relevantDate))
    return TransactionPtrList();	// No need to go further
  QDate base = document->attributes[def->relevantDate].toDate();
  return wallet.transactionsWithinRange(base.addDays(-2),
					base.addDays(def->transactionDateTolerance));
}

AtomicTransaction * Cabinet::matchingTransaction(Document * document)
{
  TransactionPtrList candidates = transactionMatchingCandidates(document);
  for(int i = 0; i < candidates.size(); i++) {
    if(document->collection->
       definition->code.scoreForTransaction(document,
                                            candidates[i]) > 1000)
      return candidates[i];
    /// \todo This is very primitive...
  }
  return NULL;
}


QList<Document *> Cabinet::documentsByType(const QString & collection, 
                                           const QString & type) 
{
  QList<Document *> retval;
  for(int i = 0; i < collections.size(); i++) {
    Collection& col = collections[i];
    if(col.definition->getName() != collection)
      continue;
    QList<Document *> docs = col.allDocuments();
    for(int j = 0; j < docs.size(); j++)
      if(docs[j]->definition->name == type)
        retval += docs[j];
  }
  return retval;
}
