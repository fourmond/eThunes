/**
    \file httarget-templates.hh
    Template classes to help having fun with HTTarget
    Copyright 2011 by Vincent Fourmond

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


#ifndef __HTTARGET_TEMPLATES_HH
#define __HTTARGET_TEMPLATES_HH

#include <utils.hh>
#include <httarget.hh>

/// Helper class to use as link target for callback members with no
/// parameters.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToMember() provides a much nicer interface.
template <class T> class HTMemberCallback0 : public HTTarget {
  T * target;
  
  typedef void (T::*MemberFunction)();
  MemberFunction targetMember;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    CALL_MEMBER_FN(*target, targetMember)();
  };

  HTMemberCallback0(T * t, MemberFunction f, bool d = true) : 
    target(t), targetMember(f), disposable(d) {;};

};

template<class T> 
QString HTTarget::linkToMember(const QString & id, 
                                      T * target,
                                      void (T::*f)() )
{
  return HTTarget::linkTo(id, new HTMemberCallback0<T>(target, f));
}

/// Helper class to use as link target for callback members with one
/// parameter.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToMember() provides a much nicer interface.
template <class T, class A1> class HTMemberCallback1 : 
  public HTTarget {
  T * target;
  
  typedef void (T::*MemberFunction)(A1 a1);
  MemberFunction targetMember;

  A1 arg1;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    CALL_MEMBER_FN(*target, targetMember)(arg1);
  };

  HTMemberCallback1(T * t, MemberFunction f, A1 a1, bool d = true) : 
    target(t), targetMember(f), arg1(a1), disposable(d) {;};

};

template<class T, class A1> 
QString HTTarget::linkToMember(const QString & id, 
                               T * target,
                               void (T::*f)(A1), A1 a1)
{
  return HTTarget::linkTo(id, new HTMemberCallback1<T, A1>(target, f, a1));
}

/// Helper class to use as link target for callback members with two
/// parameters.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToMember() provides a much nicer interface.
template <class T, class A1, class A2> class HTMemberCallback2 : 
  public HTTarget {
  T * target;
  
  typedef void (T::*MemberFunction)(A1 a1, A2 a2);
  MemberFunction targetMember;

  A1 arg1;
  A2 arg2;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    CALL_MEMBER_FN(*target, targetMember)(arg1, arg2);
  };

  HTMemberCallback2(T * t, MemberFunction f, A1 a1, A2 a2, bool d = true) : 
    target(t), targetMember(f), arg1(a1), arg2(a2), disposable(d) {;};

};

template<class T, class A1, class A2> 
QString HTTarget::linkToMember(const QString & id, 
                               T * target,
                               void (T::*f)(A1, A2), A1 a1, A2 a2)
{
  return HTTarget::
    linkTo(id, new HTMemberCallback2<T, A1, A2>(target, f, a1, a2));
}


/// Helper class to use as link target for callback to functions with one
/// parameter.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToFunction() provides a much nicer interface.
template <class A1> class HTFunctionCallback1 : 
  public HTTarget {
  
  typedef void (*Function)(A1 a1);
  Function target;

  A1 arg1;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    target(arg1);
  };

  HTFunctionCallback1(Function f, A1 a1, bool d = true) : 
    target(f), arg1(a1), disposable(d) {;};

};

template<class A1> 
QString HTTarget::linkToFunction(const QString & id, 
                                 void (*f)(A1), A1 a1)
{
  return HTTarget::linkTo(id, new HTFunctionCallback1<A1>(f, a1));
}

/// Helper class to use as link target for callback to functions with two
/// parameters.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToFunction() provides a much nicer interface.
template <class A1, class A2> class HTFunctionCallback2 : 
  public HTTarget {
  
  typedef void (*Function)(A1 a1, A2 a2);
  Function target;

  A1 arg1;
  A2 arg2;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    target(arg1, arg2);
  };

  HTFunctionCallback2(Function f, A1 a1, A2 a2, bool d = true) : 
    target(f), arg1(a1), arg2(a2), disposable(d) {;};

};

template<class A1, class A2> 
QString HTTarget::linkToFunction(const QString & id, 
                                 void (*f)(A1, A2), A1 a1, A2 a2)
{
  return HTTarget::linkTo(id, new HTFunctionCallback2<A1, A2>(f, a1, a2));
}

/// Helper class to use as link target for callback to functions with two
/// parameters.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToFunction() provides a much nicer interface.
template <class A1, class A2, class A3> class HTFunctionCallback3 : 
  public HTTarget {
  
  typedef void (*Function)(A1 a1, A2 a2, A3 a3);
  Function target;

  A1 arg1;
  A2 arg2;
  A3 arg3;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    target(arg1, arg2, arg3);
  };

  HTFunctionCallback3(Function f, A1 a1, A2 a2, A3 a3, bool d = true) : 
    target(f), arg1(a1), arg2(a2), arg3(a3), disposable(d) {;};

};

template<class A1, class A2, class A3> 
QString HTTarget::linkToFunction(const QString & id, 
                                 void (*f)(A1, A2, A3), 
                                 A1 a1, A2 a2, A3 a3)
{
  return HTTarget::
    linkTo(id, new HTFunctionCallback3<A1, A2, A3>(f, a1, a2, a3));
}

/// Helper class to use as link target for callback to functions with two
/// parameters.
///
/// In principle, you have no reasons to use this class, as
/// HTTarget::linkToFunction() provides a much nicer interface.
template <class A1, class A2, class A3, class A4> class HTFunctionCallback4 : 
  public HTTarget {
  
  typedef void (*Function)(A1 a1, A2 a2, A3 a3, A4 a4);
  Function target;

  A1 arg1;
  A2 arg2;
  A3 arg3;
  A4 arg4;

  bool disposable;

public:
  virtual bool isDisposable() const { return disposable; };
  virtual void followLink() {
    target(arg1, arg2, arg3, arg4);
  };

  HTFunctionCallback4(Function f, A1 a1, A2 a2, A3 a3, A4 a4, bool d = true) : 
    target(f), arg1(a1), arg2(a2), arg3(a3), arg4(a4), disposable(d) {;};

};

template<class A1, class A2, class A3, class A4> 
QString HTTarget::linkToFunction(const QString & id, 
                                 void (*f)(A1, A2, A3, A4), 
                                 A1 a1, A2 a2, A3 a3, A4 a4)
{
  return HTTarget::
    linkTo(id, new HTFunctionCallback4<A1, A2, A3, A4>(f, a1, a2, a3, a4));
}


#endif
