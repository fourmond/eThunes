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
#include <fetcher.hh>

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

void RubyModuleCode::ensureLoadModule()
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
  ensureLoadModule();			// Make sure the module is loaded.
  AttributeHash retVal;
  RescueMemberWrapper3Args<RubyModuleCode, const QString &, 
    const AttributeHash &, AttributeHash &>::
    wrapCall(this, &RubyModuleCode::parseDocumentMetaDataInternal,
	     doctype, contents, retVal);
  return retVal;
}

void RubyModuleCode::testDownload()
{
  ensureLoadModule();			// Make sure the module is loaded.
  RescueMemberWrapper1Arg<RubyModuleCode, int>::
    wrapCall(this, &RubyModuleCode::testDownloadInternal, 0);
}

void RubyModuleCode::testDownloadInternal(int)
{
  /// \todo This really should be done once for all in ensureLoadModule
  VALUE module = rb_eval_string((const char*)name.toLocal8Bit());
  Fetcher * n = new Fetcher();
  ID func = rb_intern("test_download");
  rb_funcall(module, func, 1, n->wrapToRuby());
}
