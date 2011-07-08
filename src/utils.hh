/**
    \file utils.hh
    Generally useful (template?) code
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


#ifndef __UTILS_HH
#define __UTILS_HH


#ifndef CALL_MEMBER_FN
/// A neat define that comes in quite useful !
#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))
#endif


/// Ensures the given list has the correct size
template <class T> void ensureSize(QList<T> & lst, int size)
{
  if(lst.size() < size) {
    while(size > lst.size())
      lst.append(T());
  }
  else {
    while(size < lst.size())
      lst.removeLast();
  }
}

/// Appends/prepends the given number of elements to the list
template <class T> void tweakSize(QList<T> & lst, int beg, int end)
{
  if(beg > 0) {
    while(beg-- > 0)
      lst.prepend(T());
  }
  else {
    while(beg++ < 0)
      lst.removeFirst();
  }

  if(end > 0) {
    while(end-- > 0)
      lst.append(T());
  }
  else {
    while(end++ < 0)
      lst.removeLast();
  }
}


#endif
