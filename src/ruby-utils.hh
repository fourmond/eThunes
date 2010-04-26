/** 
    \file ruby-utils.hh
    Various (template ?) utilities for running Ruby code.
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


#ifndef __RUBY_UTILS_HH
#define __RUBY_UTILS_HH

#include <intern.h>

/// Ruby-related functions and classes.
///
/// This namespace contains a whole bunch of Ruby-related functions and 
namespace Ruby {

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

#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember)) 

  /// Rescue exceptions from 1-arg member functions.
  ///
  /// This class can be used to run member functions taking one argument
  /// within a rescue block (in particular to avoid segfaults during the
  /// calls if an exception occurs)
  template <class C, typename Arg1> class RescueMemberWrapper1Arg {
    C * targetClass;

    typedef void (C::*MemberFunction)(Arg1 arg);
    MemberFunction targetMember;

    Arg1 arg;

    static VALUE wrapper(VALUE v) {
      RescueMemberWrapper1Arg * arg = (RescueMemberWrapper1Arg *) v;
      CALL_MEMBER_FN(*(arg->targetClass), arg->targetMember)(arg->arg);
      return Qnil;
    };


  public:
    RescueMemberWrapper1Arg(C* tc, MemberFunction tm, Arg1 a) :
      targetClass(tc), targetMember(tm), arg(a) {;};

    /// Runs the code wrapping it into a rb_rescue code
    void run() {
      rb_rescue((VALUE (*)(...)) &wrapper, (VALUE) this, 
		(VALUE (*)(...)) &globalRescueFunction, Qnil);
    };

    /// Runs a member call within
    static void wrapCall(C* tc, MemberFunction tm, Arg1 a) {
      RescueMemberWrapper1Arg c(tc,tm,a);
      c.run();
    };
  
    
  };

};

#endif
