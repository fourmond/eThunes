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

Cabinet * Cabinet::theCabinet = NULL;


Cabinet::Cabinet() : dirty(false)
{
  /// @todo Use watchChild rather, and 
  watchChild(&wallet, "wallet");
  connect(*this, SIGNAL(changed(const Watchdog *)), SLOT(setDirty()));
  connect(this, SIGNAL(collectionsPossiblyChanged()), SLOT(setDirty()));
  connect(this, SIGNAL(documentsChanged(Collection *)), SLOT(setDirty()));
  if(theCabinet)
    throw "Problem";
  theCabinet = this;
}

Cabinet::~Cabinet()
{
  theCabinet = NULL;
}

Cabinet * Cabinet::globalCabinet()
{
  return theCabinet;
}


SerializationAccessor * Cabinet::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("plugin",
		   new SerializationPointerQList<Plugin>(&plugins));
  ac->addAttribute("wallet", &wallet);
  ac->addAttribute("documents", &documents);
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

  /// @todo This should move either to Utils or as a static
  /// Serialization function.
  while(! w.isStartElement() && ! w.atEnd())
    w.readNext();
  filePath = name;
  readXML(&w);
  emit(filenameChanged(filePath));
  //  emit(collectionsPossiblyChanged());
  setDirty(false);
}


void Cabinet::clearContents()
{
  wallet.clearContents();
  // collections.clear();
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
  // rebuildDocumentsHash();
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
