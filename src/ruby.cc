/*
    ruby.cc: Various Ruby utility functions
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
#include <ruby.hh>
#include <ruby-templates.hh>

#include <exceptions.hh>
#include <fetcher.hh>

VALUE Ruby::globalRescueFunction(VALUE /*dummy*/, VALUE exception)
{
  /// \tdexception eventually, this should throw an exception
  fprintf(stderr, "A Ruby exception occured: %p\n", exception);
  VALUE inspect = rb_inspect(exception);
  fprintf(stderr, StringValueCStr(inspect));
  VALUE ct = rb_funcall2(exception, rb_intern("backtrace"), 0, NULL);
  inspect = rb_inspect(ct);
  fprintf(stderr, StringValueCStr(inspect));
  return Qnil;
}

bool Ruby::rubyInitialized = false;

VALUE Ruby::exceptionSafeCall(VALUE (*function)(...), void * args)
{
  int error;
  VALUE ret = rb_protect((VALUE (*)(VALUE)) function, 
                         (VALUE) args, &error);
  if(error) {
    // An exception occurred, we need to handle it.
    VALUE exception = rb_gv_get("$!");
    fprintf(stderr, "Exception: %p\n", exception);
    return Ruby::globalRescueFunction(Qnil, exception);
  }
  return ret;
}

QStringList valueToStringList(VALUE value);


static bool withinSafeCode = false;


static VALUE inheritedHook(VALUE self, VALUE cls)
{
  // rb_p(cls);

  // Should freeze the classes defined in the code so that a nasty
  // module cannot modify something already in place.
  //
  // Bad thing is, that prevents reload ;-)...
  return Qnil;
}

static VALUE untrustedCodeString;

static VALUE traceHook(VALUE self,
                       VALUE what, VALUE file, VALUE line,
                       VALUE funcall, VALUE binding, VALUE cls)
{
  // do something here !
  if(! withinSafeCode)
    return Qnil;
  // if( RTEST(rb_str_equal(file, untrustedCodeString))) {
    VALUE ary = rb_ary_new();
    rb_ary_push(ary, self);
    rb_ary_push(ary, what);
    rb_ary_push(ary, file);
    rb_ary_push(ary, funcall);
    rb_ary_push(ary, cls);
    //    rb_p(ary);
    // }
  return Qnil;
}

static VALUE mUtils;
ID safeCallId;


ID Ruby::fetchID;
ID Ruby::resumeID;
ID getTracerHookID;
ID setTraceFuncID;
ID checkForGlobalsID;
ID evalID;

ID Ruby::nameID;
ID Ruby::parseID;
ID Ruby::publicNameID;
ID Ruby::descriptionID;
ID Ruby::matchesID;
ID Ruby::relevantDateRangeID;
ID Ruby::cssID;
ID Ruby::newID;
ID Ruby::xpathID;




static VALUE main;

static char* argv[]  = { "ethunes-internal", "-e", "true", "-E", "utf-8:utf-8"};
static const int nbargs = sizeof(argv)/sizeof(argv[0]);


void Ruby::ensureInitRuby()
{
  if(! rubyInitialized) {
    ruby_init();
    ruby_process_options(nbargs, argv);

    untrustedCodeString = rb_eval_string("$__untrusted_code = '(untrusted code)'");

    main = rb_eval_string("self");

    loadFile("utils");
    rb_eval_string("$__safe_keeping_hash__ = {}");

    // Hook for the derivation of methods
    rb_define_singleton_method(rb_cBasicObject, "inherited",
                               (VALUE (*)(...)) inheritedHook, 1);

    mUtils = rb_eval_string("Utils");

    // Initialization of IDs
    safeCallId = rb_intern("safe_call");
    getTracerHookID = rb_intern("get_tracer_hook");
    setTraceFuncID = rb_intern("set_trace_func");
    checkForGlobalsID = rb_intern("check_for_globals");
    evalID = rb_intern("eval");
    cssID = rb_intern("css");
    nameID = rb_intern("name");
    publicNameID = rb_intern("public_name");
    descriptionID = rb_intern("description");
    matchesID = rb_intern("matches");
    relevantDateRangeID = rb_intern("relevant_date_range");
    newID = rb_intern("new");
    xpathID = rb_intern("xpath");
    

    rb_define_singleton_method(mUtils, "trace_hook",
                               (VALUE (*)(...)) traceHook, 6);

    fetchID = rb_intern("fetch");
    parseID = rb_intern("parse");
    resumeID = rb_intern("resume");
    
    Fetcher::initializeRuby();
    /// \todo Do not hardwire the list, but rather acquire it somehow
    loadFile("dates");
    loadFile("net");
    loadFile("collectiondefinition");
  }
  /// \todo Here, add a whole bunch of functions to destroy all
  /// functions for IO and process control (including require, for
  /// instance), so that the user is guaranteed that information won't
  /// leak, or that the object is not used for malicious purposes.
  rubyInitialized = true;
}

void Ruby::keepSafe(VALUE obj)
{
  VALUE gbl = rb_gv_get("$__safe_keeping_hash__");
  
  rb_hash_aset(gbl, obj, Qnil);
}

void Ruby::unKeep(VALUE obj)
{
  /// @todo !!
}

static VALUE wrapFuncall(int nb, VALUE * args)
{
  VALUE ret = rb_funcall2(mUtils, safeCallId, nb, args);

  if(BUILTIN_TYPE(ret)  != T_ARRAY)
    throw InternalError("Did not get an array");

  nb = RARRAY_LEN(ret);
  if(nb == 1)
    return rb_ary_entry(ret, 0);
  throw RubyException(rb_ary_entry(ret, 1));
  return Qnil;
}

VALUE Ruby::wrappedFuncall(VALUE tg, ID id, int number, ...)
{
  va_list args;
  VALUE tgArgs[number + 2];

  va_start(args, number);

  tgArgs[0] = tg;
  tgArgs[1] = ID2SYM(id);
  for(int i = 0; i < number; i++)
    tgArgs[i+2] = va_arg(args, VALUE);
  va_end(args);

  return wrapFuncall(number + 2, tgArgs);
}

VALUE Ruby::safeFuncall(VALUE tg, ID id, int number, ...)
{
  va_list args;
  VALUE tgArgs[number + 2];

  va_start(args, number);

  tgArgs[0] = tg;
  tgArgs[1] = ID2SYM(id);
  for(int i = 0; i < number; i++)
    tgArgs[i+2] = va_arg(args, VALUE);
  va_end(args);

  VALUE blk = wrappedFuncall(mUtils, getTracerHookID, 0);
  wrappedFuncall(main, setTraceFuncID, 1, blk);
  withinSafeCode = true;
  VALUE ret = wrapFuncall(number + 2, tgArgs);
  withinSafeCode = false;
  wrappedFuncall(main, setTraceFuncID, 1, Qnil);
  return ret;
}

static VALUE loadFileHelper(QByteArray ar)
{
  // VALUE str = rb_locale_str_new_cstr((const char *) ar);
  // rb_funcall(main, evalID, 3, str, Qnil, untrustedCodeString);
  rb_eval_string((const char*) ar);
  return Qnil;
}


void Ruby::loadFile(QString str)
{
  /// \tdexception Fire away when file is missing (or log)
  if(! str.endsWith(".rb"))	// require-like handling
    str += ".rb";
  QFile code("ruby:" + str);
  code.open(QIODevice::ReadOnly);
  QByteArray ar = code.readAll();
  run(loadFileHelper, ar);
}

void Ruby::safeLoadFile(const QString & file)
{
  QFile code(file);
  code.open(QIODevice::ReadOnly);
  QByteArray contents = code.readAll();
  VALUE str = rb_locale_str_new_cstr((const char *) contents);
  VALUE tst = wrappedFuncall(mUtils, checkForGlobalsID, 1, str);
  if(! RTEST(tst))
    throw "failure";
  safeFuncall(main, evalID, 3, str, Qnil, untrustedCodeString);
}

QStringList Ruby::valueToStringList(VALUE value)
{
  QStringList rv;
  if(! rb_obj_is_kind_of(value, rb_cArray))
    return rv;
  int ln = rb_array_len(value);
  for(int i = 0; i < ln; i++)
    rv << VALUE2QSTRING(rb_ary_entry(value, i));
  return rv;
}
