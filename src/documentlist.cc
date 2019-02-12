/*
    documentlist.cc: implementation of the DocumentList class
    Copyright 2017 by Vincent Fourmond

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
#include <documentlist.hh>

#include <document.hh>


DocumentList::DocumentList()
{
}

DocumentList::~DocumentList()
{
  for(Document * doc : documents)       // hell, I love C++11
    delete doc;
}

SerializationAccessor * DocumentList::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addHashAttribute("document", &documents);

  return ac;
}

Document * DocumentList::document(const QString & file) const
{
  return documents.value(file, NULL);
}

Document * DocumentList::modifiableDocument(const QString & file)
{
  Document * doc = documents.value(file, NULL);
  if(! doc) {
    doc = new Document(file);
    addDocument(doc);
  }
  return doc;
}

bool DocumentList::renameDocument(Document * doc, const QString & newName)
{
  QString on = doc->fileName();
  if(documents.value(on, NULL) != doc)
    return false;               /// @todo Throw
  if(doc->rename(newName)) {
    documents.remove(on);
    documents[doc->fileName()] = doc;
    return true;
  }
  return false;
}

void DocumentList::addDocument(Document * doc)
{
  QString fn = doc->fileName();
  if(fn.isEmpty())
    throw "Empty";
  if(documents.contains(fn))
    throw "Not replacing";
  documents[fn] = doc;
}

QList<Document *> DocumentList::allDocuments() const
{
  return documents.values();
}
