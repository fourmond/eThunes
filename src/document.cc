/*
    document.cc: implementation of the Document class
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
#include <document.hh>
#include <doctype.hh>

#include <cabinet.hh>
#include <periodic.hh>
#include <pdftools.hh>


Document::Document() : m_DocType(NULL)
{
}

Document::Document(const QString & fn) : m_FileName(fn), m_DocType(NULL)
{
}

QString Document::publicLinkName() const
{
  return QString("Doc: %1").arg(fileName());
}

void Document::followLink()
{
  ///
}

QString Document::docTypeName() const
{
  if(m_DocType)
    return m_DocType->name();
  return m_DocTypeName;
}

void Document::setDocTypeName(const QString & name)
{
  m_DocTypeName = name;
  m_DocType = DocType::namedType(name);
}

DocType * Document::docType() const
{
  return m_DocType;
}

QString Document::fileName() const
{
  return m_FileName;
}

void Document::setDocType(DocType * docType)
{
  m_DocType = docType;
}

bool Document::rename(const QString & newName)
{
  QDir dir = Cabinet::globalCabinet()->baseDirectory();
  QFileInfo info(newName);
  QString d = info.dir().path();
  dir.mkpath(d);
  bool rn = dir.rename(m_FileName, newName);
  if(! rn)
    return false;
  m_FileName = newName;
  return true;
}
 
SerializationAccessor * Document::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &m_FileName);
  ac->addAccessorsAttribute("type", this,
                            &Document::setDocTypeName,
                            &Document::docTypeName);
  ac->addAttribute("attributes", &attributes);
  addLinkAttributes(ac);
  addCategoriesSerialization(ac);

  return ac;
}

QString Document::filePath() const
{
  return Cabinet::globalCabinet()->baseDirectory().
    absoluteFilePath(m_FileName);
}

AttributeHash Document::readContents() const
{
  QString fn = filePath();
  return PDFTools::readPDF(fn);
}

void Document::autoDetectDocType()
{
  AttributeHash contents = readContents();
  DocType * dt = DocType::autoDetectType(contents);
  if(dt) {
    setDocType(dt);
    if(dt->hasParseMetaData())
      attributes.unite(dt->parseMetaData(contents));
  }
}

QString Document::infoText()
{
  if(! m_DocType)
    return QString();
  return attributes.formatString(m_DocType->infoFormat());
}

int Document::scoreForTransaction(AtomicTransaction * tr) const
{
  if(! m_DocType)
    return 0;
  return m_DocType->scoreForTransaction(this, tr);
}

Period Document::relevantDateRange() const
{
  if(! m_DocType)
    return Period();
  return m_DocType->relevantDateRange(this);
}
