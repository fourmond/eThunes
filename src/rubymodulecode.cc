/*
    rubymodulecode.cc: The "ruby module" collection code
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
#include <rubymodulecode.hh>

#include <attributehash.hh>

// Ruby headers;
#include <ruby.h>
#include <intern.h>

SerializationAccessor * RubyModuleCode::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("module-name", 
		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("code", 
		   new SerializationItemScalar<QString>(&code));
;
  return ac;
}

bool RubyModuleCode::hasRubyStarted = false;

void RubyModuleCode::initRuby()
{
  if(! hasRubyStarted)
    ruby_init();
  hasRubyStarted = true;
}

void RubyModuleCode::loadModule()
{
  initRuby();
  if(! moduleLoaded) {
    rb_eval_string((const char*) code.toLocal8Bit());
  }
  moduleLoaded = true;
}

RubyModuleCode::RubyModuleCode() : moduleLoaded(false) 
{
}

/// Gets the PDF contexts as a string.
///
/// \todo Probably a whole system of class should be used to handle
/// files and types
QString getPDFString(QString file)
{
  QStringList args;
  args << "-enc" << "UTF-8" << file << "-";
  QProcess p;
  p.start("pdftotext", args);
  p.waitForFinished();
  /// \todo This calls for 
  return p.readAllStandardOutput();
}

/// Wah, these are ugly wrappers; I'll have to come up with something a
/// little more decent later on...
typedef struct {
  AttributeHash * hash;
  VALUE v;
} setfromruby_wrapper;


VALUE tempo_wrapper(VALUE v)
{
  setfromruby_wrapper * w = (setfromruby_wrapper *) v;
  w->hash->setFromRuby(w->v);
  return Qnil;
}

VALUE rescue(VALUE a, VALUE b)
{
  printf("Un joli biniou !!\n");
  rb_p(a);
  rb_p(b);
  return Qnil;
}

/// Idea: for return value, get a Ruby hash, check the type of each
/// element and store it as a date in the dates hash or as a String in
/// the string hash.
void RubyModuleCode::parseDocumentMetaData(QString doctype, QString fileName)
{
  // first, convert the file from PDF into string using QProcess and
  // pdftotext.
  QString str = getPDFString(fileName);
  QTextStream o(stdout);
  o << "Contents of file: " << fileName << endl;
  o << str << endl;
  VALUE module = rb_eval_string((const char*)name.toLocal8Bit());
  VALUE rbString = rb_str_new2((const char*) str.toLocal8Bit());
  /// \todo convert - to _ in doctype !
  ID func = rb_intern((const char*)(QString("parse_") + doctype).toLocal8Bit());
  VALUE result = rb_funcall(module, func, 1, rbString);
  rb_p(result);
  // Now converting to AttributeHash and having fun
  AttributeHash a;
  setfromruby_wrapper s;
  s.hash = &a;
  s.v = result;
  // a.setFromRuby(result);
  rb_rescue((VALUE (*)(...))tempo_wrapper, (VALUE) &s, 
	    (VALUE (*)(...)) rescue, Qnil);
  a.dumpContents();
  rb_p(a.toRuby());
}
