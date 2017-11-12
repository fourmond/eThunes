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

#include <document.hh>
#include <periodic.hh>
#include <atomictransaction.hh>
#include <attributehash.hh>


// For once, I use a static type, and not a pointer, since loading of
// types is only going to happen once the 
QHash<QString, DocType*> DocType::namedTypes;

QQmlEngine * DocType::engine;

DocType::DocType(QObject * parent) : QObject(parent),
                                     parent(NULL), collection(NULL),
                                     daysBefore(10), daysAfter(20)
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

QStringList DocType::documentNames()
{
  return namedTypes.keys();
}

// void DocType::print(const QString & str)
// {
//   QTextStream o(stdout);
//   o << "[doc type: " << this << "]: " << str << endl;
// }

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

QStringList DocType::allDateFields() const
{
  QStringList p;
  if(parent)
    p = parent->allDateFields();
  return p + m_Dates;
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


QStringList DocType::allAmountFields() const
{
  QStringList p;
  if(parent)
    p = parent->allAmountFields();
  return p + m_Amounts;
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

QHash<QString, AttributeHash::HandledType> DocType::requiredAttributes()
{
  QHash<QString, AttributeHash::HandledType> rv;
  if(parent)
    rv = parent->requiredAttributes();
  for(const QString & n : m_Dates)
    rv[n] = AttributeHash::Date;
  for(const QString & n : m_Amounts)
    rv[n] = AttributeHash::Number;
  for(const QString & n : m_Strings)
    rv[n] = AttributeHash::String;
  return rv;
}


/// @todo This should be a little better, though it'll be OK for now !
/// Could be QDir::addSearchPath ?
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

QHash<Collection *, QList<DocType *> > DocType::docTypesByCollection()
{
  QHash<Collection *, QList<DocType *> > rv;
  for(DocType * dt : namedTypes.values())
    rv[dt->collection] << dt;
  return rv;
}

bool DocType::hasIsMine() const
{
  return metaObject()->indexOfMethod("isMine(QVariant)") >= 0;
}

int DocType::isMine(const AttributeHash & attrs)
{
  const QMetaObject * mo = metaObject();
  int idx = mo->indexOfMethod("isMine(QVariant)");
  if(idx < 0)
    return 0;
  QMetaMethod met = mo->method(idx);
  QVariant rv = 0;
  QVariant v = attrs.toScript();
  bool r = met.invoke(this,
             Qt::DirectConnection,
             Q_RETURN_ARG(QVariant, rv),
             Q_ARG(QVariant, v));
  return rv.toInt();
}

bool DocType::hasParseMetaData() const
{
  return metaObject()->indexOfMethod("parseMetaData(QVariant)") >= 0;
}

DocType * DocType::autoDetectType(const AttributeHash & contents)
{
  int max = 0;
  DocType * cur = NULL;
  QTextStream o(stdout);
  for(DocType * dt : namedTypes.values()) {
    o << "DocType: " << dt->m_Name << endl;
    if(dt->hasIsMine()) {
      int nb = dt->isMine(contents);
      if(nb > max) {
        max = nb;
        cur = dt;
      }
      o << " -> isMine: " << nb << endl;
    }
  }
  return cur;
}

AttributeHash DocType::parseMetaData(const AttributeHash & contents)
{
  const QMetaObject * mo = metaObject();
  int idx = mo->indexOfMethod("parseMetaData(QVariant)");
  if(idx < 0)
    return AttributeHash();
  QMetaMethod met = mo->method(idx);
  QVariant rv = 0;
  QVariant v = contents.toScript();
  bool r = met.invoke(this,
             Qt::DirectConnection,
             Q_RETURN_ARG(QVariant, rv),
             Q_ARG(QVariant, v));
  return AttributeHash::fromScript(rv);
}



static QStringList
frenchMonths(QStringList()
             << "janvier"
             << "f..?vrier"
             << "mars"
             << "avril"
             << "mai"
             << "juin"
             << "juillet"
             << "ao..?t"
             << "septembre"
             << "octobre"
             << "novembre"
             << "d..?cembre"
             );

QString DocType::getFrenchMonthRE() const
{
  return frenchMonths.join("|");
}

int DocType::parseFrenchMonth(const QString & t)
{
  for(int i = 0; i < frenchMonths.size(); i++) {
    QRegExp re(frenchMonths[i]);
    if(re.indexIn(t) >= 0)
      return i;
  }
  return -1;
}


// Very basic transaction matching
int DocType::scoreForTransaction(const Document * doc,
                                 AtomicTransaction * tr) const
{
  Period p = relevantDateRange(doc);
  if(! p.isValid())
    return 0;

  for(const QString & n : allAmountFields()) {
    if(doc->attributes.contains(n) &&
       doc->attributes[n].canConvert(QMetaType::Int)) {
      int amount = doc->attributes[n].toInt();
      if(amount == tr->getAmount() || amount + tr->getAmount() == 0)
        return 100;
    }
  }
  return 0;
}
 
Period DocType::relevantDateRange(const Document * doc) const
{
  /// @todo Delegate that to the type if the correct function exists.
  
  // We look for the first date available date in dateFields)
  for(const QString & n : allDateFields()) {
    if(doc->attributes.contains(n) &&
       doc->attributes[n].canConvert(QMetaType::QDate))
      return Period(doc->attributes[n].toDate(), daysBefore, daysAfter);
  }
  return Period();
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

void Collection::appendDocType(QQmlListProperty<DocType> *property, DocType *value)
{
  Collection * c = reinterpret_cast<Collection *>(property->data);
  c->docTypes << value;
  value->collection = c;
}

DocType * Collection::atDocType(QQmlListProperty<DocType> *property, int index)
{
  Collection * c = reinterpret_cast<Collection *>(property->data);
  return c->docTypes[index];
}

void Collection::clearDocType(QQmlListProperty<DocType> *property)
{
  Collection * c = reinterpret_cast<Collection *>(property->data);
  c->docTypes.clear();
}

int Collection::countDocType(QQmlListProperty<DocType> *property)
{
  Collection * c = reinterpret_cast<Collection *>(property->data);
  return c->docTypes.size();
}

QQmlListProperty<DocType> Collection::getDocTypes()
{
  return QQmlListProperty<DocType>(this, this,
                                   &Collection::appendDocType,
                                   &Collection::countDocType,
                                   &Collection::atDocType,
                                   &Collection::clearDocType);
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
