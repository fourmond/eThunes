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


DocType::DocType(QObject * parent) : QObject(parent)
{
}

DocType::~DocType()
{
}

void DocType::registerQMLTypes()
{
  qmlRegisterType<DocType>("DocTypes", 1, 0, "DocType");
}

QString DocType::name() const
{
  return m_Name;
}

void DocType::setName(const QString & n)
{
  m_Name = n;
}


#include <QDebug>
#include <vector>
#include <utility>
#include <algorithm>

static void dump_props(QObject *o)
{
  auto mo = o->metaObject();
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
