/**
    \file ruby.hh
    Various (template ?) utilities for running Ruby code.
    Copyright 2010, 2013 by Vincent Fourmond

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


#ifndef __RUBY_HH
#define __RUBY_HH

/// Ruby-related functions and classes.
///
/// This namespace contains a whole bunch of Ruby-related functions and
namespace Ruby {


  /// @name Global constants
  ///
  /// Used mostly for storage of IDs, classes, and the like...
  ///
  /// @{

  /// Whether Ruby has been initialized or not
  extern bool rubyInitialized;

  /// ID for :fetch
  extern ID fetchID;

  /// ID for :resume
  extern ID resumeID;

  /// ID for :name
  extern ID nameID;

  /// ID for :public_name
  extern ID publicNameID;

  /// ID for :description
  extern ID descriptionID;

  /// ID for :parse
  extern ID parseID;

  /// ID for :matches
  extern ID matchesID;

  /// ID for :relevant_date_range
  extern ID relevantDateRangeID;

  /// ID for :css
  extern ID cssID;

  /// ID for :new
  extern ID newID;

  /// ID for :xpath
  extern ID xpathID;

  /// @}

  /// A static function handling Ruby exceptions when using classes such
  /// as RescueWrapper1Arg
  ///
  /// \todo maybe the first dummy variable should be made into something
  /// useful ?
  VALUE globalRescueFunction(VALUE dummy, VALUE exception);

  /// Makes sure Ruby is initialized properly (and, in particular,
  /// loads various useful files)
  void ensureInitRuby();

  /// Loads the given file, from within the ruby prefix (in a
  /// require-like fashion somehow, but not relying on Ruby's
  /// path). This assumes Ruby has been started already.
  void loadFile(QString name);


  /// Safely loads the given file, ie make sure no malicious code can
  /// be run.
  void safeLoadFile(const QString & file);

  /// Runs a ruby function call, so that exceptions are caught and
  /// transformed into appropriate C++ functions.
  ///
  /// These functions do not use rb_protect, and hopefully will help
  /// us get around the stack unwinding barrier problem.
  VALUE wrappedFuncall(VALUE tg, ID id, int number, ...);


  /// This functions adds the given ruby object to a global hash for
  /// it not to be garbage collected.
  void keepSafe(VALUE obj);

  /// Removes the given object from global safe-keeping hash
  void unKeep(VALUE obj);

  
  /// Equivalent to wrappedFuncall, but makes sure no unsafe function
  /// are called.
  VALUE safeFuncall(VALUE tg, ID id, int number, ...);


  /// This function runs the given function (taking a void * pointer)
  /// converting Ruby exceptions into C++ exceptions when needed.
  VALUE exceptionSafeCall(VALUE (*function)(...), void * args);

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))
#endif

  /// Maybe those guys should be inline functions ?

  inline VALUE qStringToValue(const QString & str) {
    // locale is UTF-8 for Ruby !
    return rb_locale_str_new_cstr((const char*) (str).toUtf8());
  };

  inline VALUE byteArrayToValue(const QByteArray & ary) {
    return rb_locale_str_new(ary.constData(), ary.size());
  };

  inline QString valueToQString(VALUE value) {
    return QString::fromUtf8(StringValueCStr(value));
  };

  inline QString inspect(VALUE value) {
    VALUE v = rb_inspect(value);
    return QString(StringValueCStr(v));
  };

#define QSTRING2VALUE(str)			\
  Ruby::qStringToValue(str)

#define VALUE2QSTRING(value)			\
  Ruby::valueToQString(value)



  /// @name Exception-catching routines
  ///
  /// A series of functions to wrap calls to ruby functions and ensure
  /// exceptions are caught.
  ///
  /// These functions use rb_protect, and as such are not suitable for
  /// use with the continuations.
  ///
  /// These functions are defined in the ruby-templates.hh header
  ///  
  /// @{
  VALUE run(VALUE (*f)());
  template<typename A1> VALUE run(VALUE (*f)(A1), A1); 
  template<typename A1, typename A2> VALUE run(VALUE (*f)(A1, A2), A1, A2); 
  
  
  template<typename C, typename A1> VALUE run(C*, VALUE (C::*f)(A1), A1); 
  
  template<typename C, typename A1, typename A2, typename A3> VALUE run(C*, VALUE (C::*f)(A1, A2, A3), A1, A2, A3); 
  
  
  /// @}
  
};

#endif
