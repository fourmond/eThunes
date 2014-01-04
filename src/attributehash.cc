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
// Ruby-specific stuff
#include <ruby-utils.hh>

#include <ruby-templates.hh>
#include <attributehash.hh>
#include <transaction.hh>

using namespace Ruby;

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


VALUE AttributeHash::variantToRuby(const QVariant & variant)
{
  switch(variantType(variant.type())) {
  case AttributeHash::Number:
    return LONG2NUM(variant.toLongLong());
  case AttributeHash::Time:
    return rb_time_new(variant.toDateTime().toTime_t(),0 /* microseconds */);
  default: // use string value
    return QSTRING2VALUE(variant.toString());
  }
}


QVariant AttributeHash::rubyToVariant(VALUE value)
{
  if(RTEST(rb_obj_is_kind_of(value, rb_cTime))) {
    // we have a time value !
    // rb_raise(rb_eRuntimeError, "biniou !!");
    QDateTime date;
    date.setTime_t(NUM2LONG(rb_funcall(value, rb_intern("to_i"), 0)));
    return date;
  }
  if(RTEST(rb_obj_is_kind_of(value, rb_cFixnum)) ||
     RTEST(rb_obj_is_kind_of(value, rb_cBignum))) {
    // integer
    return QVariant((qlonglong) NUM2LONG(value));
  }
  // default string behavior
  return VALUE2QSTRING(value);
}

VALUE AttributeHash::toRuby() const
{
  VALUE hash = rb_hash_new();
  const_iterator i = constBegin();
  while (i != constEnd()) {
    VALUE key = QSTRING2VALUE(i.key());
    VALUE val = variantToRuby(i.value());
    rb_hash_aset(hash, key, val);
    ++i;
  }
  return hash;
}

// The syntax of rb_hash_foreach is kinda obscure, but, well, it looks
// like the function expected is to be called thus:
//
// (arg is the last argument to rb_hash_foreach
int AttributeHash::setFromRubyInternalHelper(VALUE key, VALUE val, void * arg)
{
  AttributeHash * target = static_cast<AttributeHash *>(arg);
  QString k = VALUE2QSTRING(key);
  target->operator[](k) = AttributeHash::rubyToVariant(val);
  return ST_CONTINUE;
}

VALUE AttributeHash::setFromRubyInternal(VALUE hash)
{
  rb_hash_foreach(hash, (int (*)(...))AttributeHash::
		  setFromRubyInternalHelper, (VALUE)this);
  return Qnil;
}

void AttributeHash::setFromRuby(VALUE hash, bool clr)
{
  // Rescue for potential exceptions.
  VALUE tst = rb_obj_is_kind_of(hash, rb_cHash);
  if(! RTEST(tst))
    throw "Should have been a Hash !";
  if(clr)
    clear();

  Ruby::run(this,  &AttributeHash::setFromRubyInternal, hash);
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
  const_iterator i = constBegin();
  while (i != constEnd()) {
    writer->writeStartElement(name);
    writer->writeAttribute("name", i.key());
    writer->writeAttribute("type", typeNames[variantType(i.value())]);
    /// \todo maybe date should be special-cased ?
    writer->writeAttribute("value", i.value().toString());
    writer->writeEndElement();
    i++;
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
    switch(spec[0].toAscii()) {
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
        switch(spec[0].toAscii()) {
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
