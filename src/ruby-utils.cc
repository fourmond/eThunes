/*
    ruby-utils.cc: Various Ruby utility functions
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
#include <ruby-utils.hh>
#include <ruby-templates.hh>
#include <fetcher.hh>


// For the wrapper
QApplication * ourApp;

VALUE Ruby::runMainLoop(VALUE obj)
{
  ourApp->exec();
  return Qnil;
}

VALUE mApp;

void Ruby::mainLoop(QApplication * app)
{
  ourApp = app;
  rb_eval_string("App::run");
}


VALUE Ruby::globalRescueFunction(VALUE /*dummy*/, VALUE exception)
{
  /// \tdexception eventually, this should throw an exception
  fprintf(stderr, "A Ruby exception occured:\n");
  rb_p(exception);
  VALUE ct = rb_funcall2(exception, rb_intern("backtrace"), 0, NULL);
  rb_p(ct);
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
    return Ruby::globalRescueFunction(Qnil, exception);
  }
  return ret;
}

static char * opts[] = { "-r", "continuations", NULL };

void Ruby::ensureInitRuby()
{
  if(! rubyInitialized) {
    ruby_init();
    mApp = rb_define_module("App");
    rb_define_singleton_method(mApp, "run",
                               (VALUE (*)(...)) runMainLoop, 0);


    Ruby::run(&rb_require, "continuation");
    Fetcher::initializeRuby();
    /// \todo Do not hardwire the list, but rather acquire it somehow
    loadFile("dates");
    loadFile("net");
  }
  /// \todo Here, add a whole bunch of functions to destroy all
  /// functions for IO and process control (including require, for
  /// instance), so that the user is guaranteed that information won't
  /// leak, or that the object is not used for malicious purposes.
  rubyInitialized = true;
}

void Ruby::loadFile(QString str)
{
  /// \tdexception Fire away when file is missing (or log)
  if(! str.endsWith(".rb"))	// require-like handling
    str += ".rb";
  QFile code("ruby:" + str);
  code.open(QIODevice::ReadOnly);
  rb_eval_string((const char*) code.readAll());
}
