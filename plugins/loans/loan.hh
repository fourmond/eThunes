/**
   \file loan.hh
   Plugin to keep track of loans
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


#ifndef __LOAN_HH
#define __LOAN_HH

#include <plugin.hh>
#include <linkable.hh>



/// This plugin helps to keep track of the loans one have.
class LoanPlugin : public Plugin {
public:
  virtual QString typeName() const {
    return "loan";
  };

  virtual NavigationPage * pageForPlugin();

  virtual SerializationAccessor * serializationAccessor();

  virtual void finishedSerializationRead();

  LoanPlugin();

};


#endif
