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
  virtual bool isDisposable() { return disposable; };
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


#endif
