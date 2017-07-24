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

QStringList DocType::amountFields() const
{
  return m_Amounts;
}

void DocType::setAmountFields(const QStringList & n)
{
  m_Amounts = n;
}


#include <QDebug>
#include <vector>
#include <utility>
#include <algorithm>

static void dump_props(QObject *o)
{
  const QMetaObject * mo = o->metaObject();
  qDebug() << "## Properties of" << o << "##";
  do {
    qDebug() << "### Class" << mo->className() << "###";
    std::vector<std::pair<QString, QVariant> > v;
    v.reserve(mo->propertyCount() - mo->propertyOffset());
    for (int i = mo->propertyOffset(); i < mo->propertyCount();
          ++i)
      v.emplace_back(mo->property(i).name(),
                     mo->property(i).read(o));
    std::sort(v.begin(), v.end());
    for (auto &i : v)
      qDebug() << i.first << "=>" << i.second;
  } while ((mo = mo->superClass()));

  mo = o->metaObject();
  qDebug() << "## Methods of" << o << "##";
  do {
    qDebug() << "### Class" << mo->className() << "###";
    std::vector<std::pair<QString, QVariant> > v;
    v.reserve(mo->methodCount() - mo->methodOffset());
    for (int i = mo->methodOffset(); i < mo->methodCount();
          ++i)
      v.emplace_back(mo->method(i).name(),
                     mo->method(i).methodSignature());
    std::sort(v.begin(), v.end());
    for (auto &i : v)
      qDebug() << i.first << "=>" << i.second;
  } while ((mo = mo->superClass()));
}


void DocType::parseQMLFile(const QString & file)
{
  QQmlEngine *engine = new QQmlEngine;
  QQmlComponent component(engine, QUrl::fromLocalFile(file));
  if(component.isError()) {
    QList<QQmlError> errs = component.errors();
    QTextStream o(stdout);
    for(int i = 0; i < errs.size();i++)
      o << errs[i].toString() << endl;
  }
  else {
    QObject *myObject = component.create();
    dump_props(myObject);
  }
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
