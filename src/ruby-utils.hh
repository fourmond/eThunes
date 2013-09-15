/**
    \file ruby-utils.hh
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


#ifndef __RUBY_UTILS_HH
#define __RUBY_UTILS_HH

/// Ruby-related functions and classes.
///
/// This namespace contains a whole bunch of Ruby-related functions and
namespace Ruby {

  /// Whether Ruby has been initialized or not
  extern bool rubyInitialized;

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

  /// Run the application
  void mainLoop(QApplication * app);

  /// 
  VALUE runMainLoop(VALUE obj);


  /// This function runs the given function (taking a void * pointer)
  /// converting Ruby exceptions into C++ exceptions when needed.
  VALUE exceptionSafeCall(VALUE (*function)(...), void * args);

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))
#endif

  /// Maybe those guys should be inline functions ?

  inline VALUE qStringToValue(const QString & str) {
    return rb_str_new2((const char*) (str).toLocal8Bit());
  };

  inline VALUE byteArrayToValue(const QByteArray & ary) {
    return rb_str_new(ary.constData(), ary.size());
  };

  inline QString valueToQString(VALUE value) {
    return QString(StringValueCStr(value));
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
  /// These functions are defined in the ruby-templates.hh header
  ///  
  /// @{
  VALUE run(VALUE (*f)());
  template<typename A1> VALUE run(VALUE (*f)(A1), A1); 
  template<typename A1, typename A2> VALUE run(VALUE (*f)(A1, A2), A1, A2); 


  template<typename C, typename A1> VALUE run(C*, VALUE (C::*f)(A1), A1); 

  template<typename C, typename A1, typename A2, typename A3> VALUE run(C*, VALUE (C::*f)(A1, A2, A3), A1, A2, A3); 


  /// @}


  /// Rescue exceptions from 2-arg non-member functions.
  template <typename Arg1, typename Arg2> class RescueWrapper2Args {

    typedef VALUE (*Function)(Arg1 arg1, Arg2 arg2);
    Function targetFunction;

    Arg1 arg1;
    Arg2 arg2;

    static VALUE wrapper(VALUE v) {
      RescueWrapper2Args * arg = (RescueWrapper2Args *) v;
      return arg->targetFunction(arg->arg1, arg->arg2);
    };


  public:
    RescueWrapper2Args(Function tf, Arg1 a1, Arg2 a2) :
      targetFunction(tf), arg1(a1), arg2(a2) {;};

    /// Runs the code wrapping it into a rb_rescue code
    void run() {
      rb_rescue((VALUE (*)(...)) &wrapper, (VALUE) this,
		(VALUE (*)(...)) &globalRescueFunction, Qnil);
    };

    /// Runs a member call within
    static void wrapCall(Function tf, Arg1 a1, Arg2 a2) {
      RescueWrapper2Args c(tf,a1,a2);
      c.run();
    };
  };


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

  /// Rescue exceptions for 3-arg member functions.
  ///
  /// This class can be used to run member functions taking one argument
  /// within a rescue block (in particular to avoid segfaults during the
  /// calls if an exception occurs)
  template <class C, typename Arg1, typename Arg2, typename Arg3>
  class RescueMemberWrapper3Args {
    C * targetClass;

    typedef void (C::*MemberFunction)(Arg1 a1, Arg2 a2, Arg3 a3);
    MemberFunction targetMember;

    Arg1 arg1;
    Arg2 arg2;
    Arg3 arg3;

    static VALUE wrapper(VALUE v) {
      RescueMemberWrapper3Args * arg = (RescueMemberWrapper3Args *) v;
      CALL_MEMBER_FN(*(arg->targetClass), arg->targetMember)(arg->arg1,
							     arg->arg2,
							     arg->arg3);
      return Qnil;
    };


  public:
    RescueMemberWrapper3Args(C* tc, MemberFunction tm, Arg1 a1, Arg2 a2, Arg3 a3) :
      targetClass(tc), targetMember(tm), arg1(a1), arg2(a2), arg3(a3) {;};

    /// Runs the code wrapping it into a rb_rescue code
    void run() {
      rb_rescue((VALUE (*)(...)) &wrapper, (VALUE) this,
		(VALUE (*)(...)) &globalRescueFunction, Qnil);
    };

    /// Runs a member call within
    static void wrapCall(C* tc, MemberFunction tm, Arg1 a1,Arg2 a2, Arg3 a3) {
    RescueMemberWrapper3Args c(tc,tm,a1,a2,a3);
      c.run();
    };


  };

};

#endif
