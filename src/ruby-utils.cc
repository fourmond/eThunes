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
#include <fetcher.hh>

VALUE Ruby::globalRescueFunction(VALUE /*dummy*/, VALUE exception)
{
  /// \tdexception eventually, this should throw an exception
  fprintf(stderr, "A Ruby exception occured:\n");
  rb_p(exception);
  return Qnil;
}

bool Ruby::rubyInitialized = false;


void Ruby::ensureInitRuby()
{
  if(! rubyInitialized) {
    ruby_init();
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
