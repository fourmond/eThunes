/*
    result.cc: implementation of the Result class
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
// Ruby-specific stuff
#include <ruby-utils.hh>
#include <result.hh>

using namespace Ruby;


bool Result::rubyInitialized = false;

VALUE Result::cResult;
  
Result::Result(QNetworkReply * r) : reply(r)
{
  data = reply->readAll();
}

void Result::rubyFree(VALUE v)
{
  Result * f;
  Data_Get_Struct(v,Result,f);
  delete f;
}


VALUE Result::wrapToRuby()
{
  return Data_Wrap_Struct(cResult, NULL, rubyFree, this);
}

VALUE Result::contentsAccessor(VALUE v)
{
  Result * f;
  Data_Get_Struct(v,Result,f);
  return byteArrayToValue(f->data);
}

void Result::initializeRuby(VALUE mNet)
{
  if(rubyInitialized)
    return;

  cResult = rb_define_class_under(mNet, "Result", rb_cObject);

  rb_define_method(cResult, "contents", 
  		   (VALUE (*)(...)) contentsAccessor, 0);


  rubyInitialized = true;
}
