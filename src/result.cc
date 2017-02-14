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
// #include <ruby.hh>
#include <result.hh>

using namespace Ruby;


bool Result::rubyInitialized = false;

VALUE Result::cResult;

Result::Result(QNetworkReply * r) : reply(r), nokoCache(Qnil)
{
  data = reply->readAll();
}

void Result::rubyFree(VALUE v)
{
  if(TYPE(v) == RUBY_T_DATA) {
    Result * f = fromValue(v);
    delete f;
  }
  else {
    // For some reasons, this function gets called on VALUE objects
    // whose type is not RUBY_T_DATA. Ignoring for now, but that means
    // memory leaks in the end.
    //
    // And it means that we have some wild stack corruption
    // here...
    fprintf(stderr, "Object: %p with invalid type: %d\n", v, TYPE(v));
  }
}

Result::~Result()
{
  // I somehow doubt it's being called ?
  if(RTEST(nokoCache))
    Ruby::unKeep(nokoCache);
}

VALUE Result::nokogiriHandle()
{
  if(! RTEST(nokoCache)) {
    nokoCache = Ruby::wrappedFuncall(cNokogiriHTML, parseID, 1, 
                                     byteArrayToValue(data));
    keepSafe(nokoCache);
  }
  return nokoCache;
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

VALUE Result::css(VALUE v, VALUE css)
{
  Result * f = fromValue(v);
  return wrappedFuncall(f->nokogiriHandle(), cssID, 1, css);
}

VALUE Result::xpath(VALUE v, VALUE xpath)
{
  Result * f = fromValue(v);
  return wrappedFuncall(f->nokogiriHandle(), xpathID, 1, xpath);
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

QString Result::normalizeURL(const QString & url) const
{
  QUrl tst(url);
  if(! tst.isRelative())        // Not relative, dropping
    return url;
  QUrl org = reply->url();
  tst.setScheme(org.scheme());
  tst.setHost(org.host());
  if(org.port() >= 0)
    tst.setPort(org.port());
  return tst.toEncoded();
}

VALUE Result::normalizeURL(VALUE v, VALUE url)
{
  Result * f = fromValue(v);
  return qStringToValue(f->normalizeURL(VALUE2QSTRING(url)));
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

VALUE Result::cNokogiriHTML = Qnil;



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

  rb_define_method(cResult, "normalize_url",
  		   (VALUE (*)(...)) 
                   (static_cast<VALUE (*)(VALUE, VALUE)>(&Result::normalizeURL)), 1);

  rb_define_method(cResult, "css",
  		   (VALUE (*)(...)) css, 1);

  rb_define_method(cResult, "xpath",
  		   (VALUE (*)(...)) xpath, 1);

  rb_define_method(cResult, "ok?",
  		   (VALUE (*)(...)) wentOK, 0);

  rb_define_method(cResult, "error",
  		   (VALUE (*)(...)) errorString, 0);

  rb_define_method(cResult, "inspect",
  		   (VALUE (*)(...)) inspect, 0);


  // Initialize the Nokogiri stuff:
  if(RTEST(rb_eval_string("$__have_nokogiri__"))) {
    cNokogiriHTML = rb_eval_string("Nokogiri::HTML");
  }

  rubyInitialized = true;
}

QTemporaryFile * Result::saveToTemporaryFile() const
{
  QTemporaryFile * file = new QTemporaryFile;
  file->open();
  saveReply(file);
  return file;
}
