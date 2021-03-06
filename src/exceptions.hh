/**
   \file exceptions.hh
   Exceptions handling in eThunes
   Copyright 2012 by Vincent Fourmond

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
#ifndef __EXCEPTIONS_HH
#define __EXCEPTIONS_HH

class Exception : public std::exception {
  QString msg;
public:
  Exception(const QString & msg) throw();
  virtual const char * what() const throw();
  virtual QString message() const throw();
  virtual ~Exception() throw() {;};

};

class RuntimeError : public Exception {
public:
  RuntimeError(const QString & msg) throw() : Exception(msg) {
  };
  virtual ~RuntimeError() throw() {;};
};

class InternalError : public Exception {
public:
  InternalError(const QString & msg) throw() : Exception(msg) {
  };
  virtual ~InternalError() throw() {;};
};


#endif
