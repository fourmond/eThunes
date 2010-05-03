/*
    cabinet.cc: The Cabinet class
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


Cabinet::Cabinet() : dirty(false)
{
  connect(&wallet, SIGNAL(accountsChanged()), SLOT(setDirty()));
  connect(&wallet, SIGNAL(categoriesChanged()), SLOT(setDirty()));
  connect(&wallet, SIGNAL(filtersChanged()), SLOT(setDirty()));
}


SerializationAccessor * Cabinet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("wallet", &wallet);
  ac->addAttribute("collection", 
		   new SerializationQList<Collection>(&collections));
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
  /// \tdexception Raise on NULL CollectionDefinition.
  return c;
}

