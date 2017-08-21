/*
    attributehash.hh: implementation of the AttributeHash class
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
#include <attributehash.hh>

#include <transaction.hh>

const char * AttributeHash::typeNames[] = {
  "string", "number", "time", 0
};

AttributeHash::HandledType AttributeHash::variantType(const QVariant &variant)
{
  switch(variant.type()) {
  case QVariant::UInt:
  case QVariant::Int:
  case QVariant::LongLong:
  case QVariant::ULongLong:
    /// \todo there are risks of precision loss for large numbers here
    /// (over 2 billions)
    return AttributeHash::Number;
  case QVariant::Date:
  case QVariant::DateTime:
  case QVariant::Time: // time values
    return AttributeHash::Time;
  default: // use string value
    return AttributeHash::String;
  }
}


void AttributeHash::dumpContents() const
{
  QTextStream o(stdout);
  const_iterator i = constBegin();
  while (i != constEnd()) {
    o << "Key: " << i.key() << ": (type "
      << typeNames[variantType(i.value())] << ") "
      << i.value().toString() << endl;
    i++;
  }

}

void AttributeHash::writeXML(const QString& name,
			     QXmlStreamWriter* writer)
{
  QStringList ks = keys();
  qSort(ks);
  for(int i = 0; i < ks.size(); i++) {
    const QString & k = ks[i];
    writer->writeStartElement(name);
    writer->writeAttribute("name", k);
    writer->writeAttribute("type", typeNames[variantType(value(k))]);
    /// \todo maybe date should be special-cased ?
    writer->writeAttribute("value", value(k).toString());
    writer->writeEndElement();
  }
}

void AttributeHash::readXML(QXmlStreamReader* reader)
{
  // Once again, this function only reads one element; assumes to be
  // just at the beginning of the element.
  QXmlStreamAttributes attrs = reader->attributes();
  QString name = attrs.value("name").toString();
  HandledType t = namedType(attrs.value("type").toString());
  QString value = attrs.value("value").toString();
  Serialization::readNextToken(reader);
  switch(t) {
  case String:
    operator[](name) = QVariant(value);
    return;
  case Number:
    operator[](name) = QVariant(value.toLongLong());
    return;
  case Time:
    operator[](name) = QDateTime::fromString(value, Qt::ISODate);
    return;
  }
}

static const char * fsRE = "%\\{([^%}]+)(%[^}]+)?\\}";

QString AttributeHash::formatString(const QString & format) const
{
  QString str;
  int idx = 0;
  int lastidx;
  QRegExp formatSpecifierRE(fsRE);
  // formatSpecifierRE.setMinimal(true); // Non-greedy regular expressions !
  while(idx >= 0) {
    lastidx = idx;
    idx = formatSpecifierRE.indexIn(format, idx);
    if(idx < 0) {
      str += format.mid(lastidx);
    }
    else {
      // prefix.
      str += format.mid(lastidx, idx - lastidx);
      // Now, we have fun
      if(! contains(formatSpecifierRE.cap(1)))
	str += "undef";
      else {
	// handle formats !
	if(formatSpecifierRE.cap(2).isEmpty())
	  str += value(formatSpecifierRE.cap(1)).toString();
	else
	  str += formatVariant(value(formatSpecifierRE.cap(1)),
			       formatSpecifierRE.cap(2).mid(1));
      }
      idx += formatSpecifierRE.cap(0).size();
     }
  }
  return str;
}


QString AttributeHash::formatVariant(QVariant v, const QString &spec)
{
  if(spec.size() == 1) {
    switch(spec[0].toLatin1()) {
    case 'A':
      return Transaction::formatAmount(v.toLongLong());
    case 'M':
      return v.toDateTime().toString("MM");
    case 'y':
      return v.toDateTime().toString("yyyy");
    default:
      return v.toString();
    }
  }
  else if(spec.startsWith("date:"))
    return v.toDateTime().toString(spec.mid(5));
  else
    return v.toString();
}

/// @todo This function takes code from both the one below and the one
/// above. There should be a way to refactor all this, but, well...
QHash<QString, AttributeHash::HandledType> AttributeHash::requiredAttributes(const QString & format)
{
  QHash<QString, AttributeHash::HandledType> ret;
  int idx = 0;
  QRegExp formatSpecifierRE(fsRE);
  while(true) {
    idx = formatSpecifierRE.indexIn(format, idx);
    if(idx < 0)
      break;
    HandledType t = String;
    QString spec = formatSpecifierRE.cap(2);
    if(! spec.isEmpty()) {
      if(spec.size() == 1) {
        switch(spec[0].toLatin1()) {
        case 'A':
          t = Number;
          break;
        case 'M':
        case 'y':
          t = Time;
        default:
          ;
        }
      }
      else if(spec.startsWith("date:"))
        t = Time;
    }
    ret[formatSpecifierRE.cap(1)] = t;
    idx += formatSpecifierRE.matchedLength();
  }
  return ret;
}


AttributeHash::HandledType AttributeHash::namedType(const QString & name)
{
  int i = String;
  const char ** ptr = typeNames;
  while(*ptr) {
    if(name == *ptr)
      return (HandledType) i;
    i++;
    ptr++;
  }
  /// @tcexception Issue a warning / throw an exception when name unknown ?
  return String; 		// By default
}

QWidget * AttributeHash::createEditor(AttributeHash::HandledType type, 
                                      QWidget * parent)
{
  switch(type) {
  case String: 
    return new QLineEdit(parent);
  case Number:
    return new QLineEdit(parent);
  case Time: {
    QDateTimeEdit * de = new QDateTimeEdit(parent);
    de->setCalendarPopup(true);
    return de;
  }
  }
  return NULL;
}

void AttributeHash::setEditorValue(AttributeHash::HandledType type, 
                                   QWidget * editor, 
                                   const QVariant &value)
{
  switch(type) {
  case String:
  case Number:  {
    QLineEdit * le = dynamic_cast<QLineEdit *>(editor);
    if(! le)
      throw "Invalid editor...";
    le->setText(value.toString());
    return;
  }
  case Time: {
    QDateTimeEdit * de = dynamic_cast<QDateTimeEdit *>(editor);
    if(! de)
      throw "Invalid editor...";
    de->setDateTime(value.toDateTime());
    return;
  }
    
  };
}

QVariant AttributeHash::getEditorValue(AttributeHash::HandledType type, 
                                       QWidget * editor)
{
  switch(type) {
  case String:
  case Number:  {
    QLineEdit * le = dynamic_cast<QLineEdit *>(editor);
    if(! le)
      throw "Invalid editor...";
    if(type == String)
      return QVariant(le->text());
    else
      return QVariant(le->text().toInt());
  }
  case Time: {
    QDateTimeEdit * de = dynamic_cast<QDateTimeEdit *>(editor);
    if(! de)
      throw "Invalid editor...";
    return de->dateTime();
  }
  }
  return QVariant();
}

void AttributeHash::connectEditorChanged(HandledType type, QWidget * editor,
                                         QObject * target, const char * slot)
{
  switch(type) {
  case String:
  case Number:  {
    QLineEdit * le = dynamic_cast<QLineEdit *>(editor);
    if(! le)
      throw "Invalid editor...";
    QObject::connect(le, SIGNAL(textEdited(const QString)), target, slot);
    return;
  }
  case Time: {
    QDateTimeEdit * de = dynamic_cast<QDateTimeEdit *>(editor);
    if(! de)
      throw "Invalid editor...";
    QObject::connect(de, SIGNAL(dateTimeChanged(const QDateTime &)),
                     target, slot);
    return;
  }
  }
}

QVariant AttributeHash::toScript() const
{
  QVariantMap m;
  for(const QString & n : keys())
    m.insert(n, (*this)[n]);
  return QVariant::fromValue(m);
}

AttributeHash AttributeHash::fromScript(const QVariant & value)
{
  QVariantMap m = value.toMap();
  AttributeHash rv;
  for(const QString & n : m.keys())
    rv[n] = m[n];
  return rv;
}

