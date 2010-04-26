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

// Ruby headers;
#include <ruby-utils.hh>

using namespace Ruby;

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

void RubyModuleCode::loadModule()
{
  Ruby::ensureInitRuby();
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
  /// \tdexception This calls for proper error handling
  return p.readAllStandardOutput();
}

void RubyModuleCode::parseDocumentMetaDataInternal(const QString & doctype,
						   const AttributeHash & contents,
						   AttributeHash & target)
{
  VALUE module = rb_eval_string((const char*)name.toLocal8Bit());
  VALUE hash = contents.toRuby();
  ID func = rb_intern((const char*)(QString("parse_") + doctype).toLocal8Bit());
  VALUE result = rb_funcall(module, func, 1, hash);
  target.setFromRuby(result);
}

AttributeHash RubyModuleCode::parseDocumentMetaData(const QString &doctype, 
						    const AttributeHash & contents)
{
  AttributeHash retVal;
  /// \todo Handle Ruby exceptions !!
  parseDocumentMetaDataInternal(doctype, contents, retVal);
  return retVal;
}
