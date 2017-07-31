/*
    doctype.cc: implementation of the  DocType class 
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
#include <doctype.hh>


// For once, I use a static type, and not a pointer, since loading of
// types is only going to happen once the 
QHash<QString, DocType*> DocType::namedTypes;

QQmlEngine * DocType::engine;

DocType::DocType(QObject * parent) : QObject(parent)
{
}

DocType::~DocType()
{
}

void DocType::registerQMLTypes()
{
  qmlRegisterType<DocType>("DocTypes", 1, 0, "DocType");
  qmlRegisterType<Collection>("DocTypes", 1, 0, "Collection");
}

QString DocType::name() const
{
  return m_Name;
}

void DocType::setName(const QString & n)
{
  if(m_Name == n)
    return;
  m_Name = n;
  if(namedTypes.contains(m_Name)) {
    /// @todo Logging !
    QTextStream o(stderr);
    o << "Redefining doc type '" << m_Name << "'" << endl;
    delete namedTypes.take(m_Name);
  }
  namedTypes[m_Name] = this;
}

QString DocType::description() const
{
  return m_Description;
}

void DocType::setDescription(const QString & n)
{
  m_Description = n;
}

QString DocType::infoFormat() const
{
  return m_InfoFormat;
}

void DocType::setInfoFormat(const QString & n)
{
  m_InfoFormat = n;
}


QString DocType::parentName() const
{
  if(parent)
    return parent->name();      // ??
  return m_ParentName;
}

void DocType::setParentName(const QString & n)
{
  m_ParentName = n;
}

QStringList DocType::dateFields() const
{
  return m_Dates;
}

void DocType::setDateFields(const QStringList & n)
{
  m_Dates = n;
}

QStringList DocType::stringFields() const
{
  return m_Strings;
}

void DocType::setStringFields(const QStringList & n)
{
  m_Strings = n;
}

QStringList DocType::amountFields() const
{
  return m_Amounts;
}

void DocType::setAmountFields(const QStringList & n)
{
  m_Amounts = n;
}


void DocType::parseQMLFile(const QString & file)
{
  if(! engine)
    engine = new QQmlEngine;
  QQmlComponent component(engine, QUrl::fromLocalFile(file));
  if(component.isError()) {
    QList<QQmlError> errs = component.errors();
    QTextStream o(stdout);
    for(int i = 0; i < errs.size();i++)
      o << errs[i].toString() << endl;
  }
  else {
    component.create();
  }
}

DocType * DocType::namedType(const QString & name)
{
  return namedTypes.value(name, NULL);
}

void DocType::crosslinkTypes()
{
  for(auto i : namedTypes) {
    if(! i->m_ParentName.isEmpty()) {
      i->parent = namedType(i->m_ParentName);
    }
  }
}


/// @todo This should be a little better, though it'll be OK for now !
/// Could be QDir::addSeachPath ?
static QStringList loadPaths("/home/vincent/Prog/eThunes/collections");

void DocType::loadTypes()
{
  for(const QString & dir : ::loadPaths) {
    QDir d(dir);
    for(auto f : d.entryList(QStringList("*.doctype.qml")))
      parseQMLFile(d.absoluteFilePath(f));
  }
  crosslinkTypes();
}



//////////////////////////////////////////////////////////////////////

Collection::Collection(QObject * parent) : QObject(parent)
{
}

Collection::~Collection()
{
}

QString Collection::name() const
{
  return m_Name;
}

QQmlListProperty<DocType> Collection::getDocTypes()
{
  return QQmlListProperty<DocType>(this, docTypes);
}

void Collection::setName(const QString & n)
{
  m_Name = n;
}

QString Collection::description() const
{
  return m_Description;
}

void Collection::setDescription(const QString & n)
{
  m_Description = n;
}
