/**
   \file ruby-templates.hh
   Templates to be used to catch exceptions within calls to Ruby
   Copyright 2013 by Vincent Fourmond

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
#ifndef __RUBY_TEMPLATES_HH
#define __RUBY_TEMPLATES_HH

#include <ruby-utils.hh>

/// Argumentless callback
class RubyCallback0  {
  typedef VALUE (*Callback)();
  Callback callback;

  static VALUE wrapper(VALUE v) {
    RubyCallback0 * arg = (RubyCallback0 *) v;
    return arg->callback();
  };
public:

  RubyCallback0(Callback c) : callback(c) {;};

  /// Runs the code wrapping it into a rb_rescue code
  VALUE run() {
    return Ruby::exceptionSafeCall((VALUE (*)(...)) &wrapper, this);
  };

};

inline VALUE Ruby::run(VALUE (*f)())
{
  RubyCallback0 cb(f);
  return cb.run();
}

/// Callback with one argument
template <typename A1> class RubyCallback1  {
  typedef VALUE (*Callback)(A1);
  Callback callback;
  A1 a1;

  static VALUE wrapper(VALUE v) {
    RubyCallback1 * arg = (RubyCallback1 *) v;
    return arg->callback(arg->a1);
  };
public:

  RubyCallback1(Callback c, A1 arg1) : callback(c), a1(arg1) {;};

  /// Runs the code wrapping it into a rb_rescue code
  VALUE run() {
    return Ruby::exceptionSafeCall((VALUE (*)(...)) &wrapper, this);
  };

};

template<typename A1> VALUE Ruby::run(VALUE (*f)(A1), A1 a1)
{
  RubyCallback1<A1> cb(f, a1);
  return cb.run();
}

/// Callback with two arguments
template <typename A1, typename A2> class RubyCallback2  {
  typedef VALUE (*Callback)(A1, A2);
  Callback callback;
  A1 a1;
  A2 a2;

  static VALUE wrapper(VALUE v) {
    RubyCallback2 * arg = (RubyCallback2 *) v;
    return arg->callback(arg->a1, arg->a2);
  };
public:

  RubyCallback2(Callback c, A1 arg1, A2 arg2) : 
    callback(c), a1(arg1), a2(arg2) {;};

  /// Runs the code wrapping it into a rb_rescue code
  VALUE run() {
    return Ruby::exceptionSafeCall((VALUE (*)(...)) &wrapper, this);
  };

};

template<typename A1, typename A2> VALUE Ruby::run(VALUE (*f)(A1, A2), 
                                                   A1 a1, A2 a2)
{
  RubyCallback2<A1, A2> cb(f, a1, a2);
  return cb.run();
}

/// Rescue exceptions from 1-arg member functions.
///
/// Use it via the Ruby::run appropriate wrapper.
template <class C, typename Arg1> class RubyMember1 {
  C * targetClass;

  typedef VALUE (C::*MemberFunction)(Arg1 arg);
  MemberFunction targetMember;

  Arg1 arg;

  static VALUE wrapper(VALUE v) {
    RubyMember1 * arg = (RubyMember1 *) v;
    return CALL_MEMBER_FN(*(arg->targetClass), arg->targetMember)(arg->arg);
  };


public:
  RubyMember1(C* tc, MemberFunction tm, Arg1 a) :
    targetClass(tc), targetMember(tm), arg(a) {;};

  VALUE run() {
    return Ruby::exceptionSafeCall((VALUE (*)(...)) &wrapper, 
                                   this);
  };

};

template<typename C, typename A1> VALUE Ruby::run(C *cls, 
                                                  VALUE (C::*f)(A1), 
                                                  A1 a1)
{
  RubyMember1<C, A1> cb(cls, f, a1);
  return cb.run();
}





#endif
