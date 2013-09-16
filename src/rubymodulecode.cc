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
#include <ruby-templates.hh>

#include <exceptions.hh>

using namespace Ruby;

SerializationAccessor * RubyModuleCode::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("module-name", &name);
  ac->addScalarAttribute("code", &code, false);
  return ac;
}

void RubyModuleCode::ensureLoadModule()
{
  Ruby::ensureInitRuby();
  if(! module) {
    rb_eval_string((const char*) code.toLocal8Bit());
  }
  /// \todo Maybe this should eventually use rb_const_get one day ?
  module = rb_eval_string((const char*)name.toLocal8Bit());
}

RubyModuleCode::RubyModuleCode() : module(0)
{
}

void RubyModuleCode::parseDocumentMetaDataInternal(const QString & doctype,
						   const AttributeHash & contents,
						   AttributeHash & target)
{
  QString funcName = QString("parse_") + doctype;
  funcName.replace("-","_");
  ensureLoadModule();
  VALUE hash = contents.toRuby();
  ID func = rb_intern((const char*)funcName.toLocal8Bit());
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

bool RubyModuleCode::canFetch()
{
  ensureLoadModule();
  return rb_respond_to(module, rb_intern("fetch"));
}

VALUE RubyModuleCode::fetchNewDocumentsInternal(const AttributeHash & credentials,
                                                const QList<AttributeHash> &existingDocuments, 
                                                Fetcher * f)
{
  ensureLoadModule();

  ID func = rb_intern("fetch");
  VALUE ary = rb_ary_new();
  for(int i = 0; i < existingDocuments.size(); i++)
    rb_ary_push(ary, existingDocuments[i].toRuby());
  
  VALUE fiber = 
    Ruby::wrappedFuncall(rb_eval_string("Net"), func, 4, module, 
                         f->wrapToRuby(),
                         credentials.toRuby(), ary);
  return fiber;
}

Fetcher * RubyModuleCode::fetchNewDocuments(const AttributeHash & credentials,
                                            const QList<AttributeHash> &existingDocuments, 
                                            Collection * c)
{
  Fetcher * n = new Fetcher();
  n->setTarget(c);

  VALUE fiber = fetchNewDocumentsInternal(credentials, 
                                          existingDocuments,
                                          n);
  
  n->setFiber(fiber);
  Ruby::wrappedFuncall(fiber, rb_intern("resume"), 0);
  return n;
}
