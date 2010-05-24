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
  Result * f = fromValue(v);
  delete f;
}


VALUE Result::wrapToRuby()
{
  return Data_Wrap_Struct(cResult, NULL, rubyFree, this);
}

VALUE Result::contentsAccessor(VALUE v)
{
  Result * f = fromValue(v);
  return byteArrayToValue(f->data);
}

VALUE Result::rawHeadersAccessor(VALUE v)
{
  Result * f = fromValue(v);
  VALUE hash = rb_hash_new();
  QList<QByteArray> headers = f->reply->rawHeaderList();
  for(int i = 0; i < headers.size(); i++)
    rb_hash_aset(hash, byteArrayToValue(headers[i]),
		 byteArrayToValue(f->reply->rawHeader(headers[i])));
  return hash;
}

VALUE Result::urlAccessor(VALUE v)
{
  Result * f = fromValue(v);
  return byteArrayToValue(f->reply->url().
			  toEncoded(QUrl::StripTrailingSlash));
}

VALUE Result::wentOK(VALUE v)
{
  Result * f = fromValue(v);
  if(f->reply->error() == QNetworkReply::NoError)
    return Qtrue;
  return Qfalse;
}

VALUE Result::errorString(VALUE v)
{
  Result * f = fromValue(v);
  return qStringToValue(f->reply->errorString());
}

VALUE Result::inspect(VALUE v)
{
  Result * f = fromValue(v);
  QString retval = QString().sprintf("#<Net::Result 0x%lx ",(unsigned long) v);
  retval += "url: '";
  retval += f->reply->url().toString() + "' ";
  if(f->reply->error() == QNetworkReply::NoError)
    retval += "ok";
  else
    retval += "error: " + f->reply->errorString();
  retval += ">";
  return qStringToValue(retval);
}


void Result::initializeRuby(VALUE mNet)
{
  if(rubyInitialized)
    return;

  cResult = rb_define_class_under(mNet, "Result", rb_cObject);

  rb_define_method(cResult, "contents",
  		   (VALUE (*)(...)) contentsAccessor, 0);

  rb_define_method(cResult, "raw_headers",
  		   (VALUE (*)(...)) rawHeadersAccessor, 0);

  rb_define_method(cResult, "url",
  		   (VALUE (*)(...)) urlAccessor, 0);

  rb_define_method(cResult, "ok?",
  		   (VALUE (*)(...)) wentOK, 0);

  rb_define_method(cResult, "error",
  		   (VALUE (*)(...)) errorString, 0);

  rb_define_method(cResult, "inspect",
  		   (VALUE (*)(...)) inspect, 0);


  rubyInitialized = true;
}
