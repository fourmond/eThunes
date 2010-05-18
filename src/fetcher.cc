/*
    fetcher.cc: implementation of the Fetcher class
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
#include <fetcher.hh>
#include <result.hh>

using namespace Ruby;


bool Fetcher::rubyInitialized = false;

VALUE Fetcher::mNet;
  
VALUE Fetcher::cFetcher;
  
Fetcher::Fetcher()
{
  manager = new QNetworkAccessManager(this);
  cookieJar = new QNetworkCookieJar();
  manager->setCookieJar(cookieJar);
  manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
  connect(manager, SIGNAL(finished(QNetworkReply*)),
	  this, SLOT(replyFinished(QNetworkReply*)));
}

void Fetcher::rubyFree(VALUE v)
{
  Fetcher * f;
  Data_Get_Struct(v,Fetcher,f);
  delete f;
}

VALUE Fetcher::wrapToRuby()
{
  return Data_Wrap_Struct(cFetcher, NULL, rubyFree, this);
}

void Fetcher::initializeRuby()
{
  if(rubyInitialized)
    return;

  mNet = rb_define_module("Net");
  cFetcher = rb_define_class_under(mNet, "Fetcher", rb_cObject);

  /// \todo define class functions.
  rb_define_method(cFetcher, "get", 
		   (VALUE (*)(...)) getWrapper, 1);


  Result::initializeRuby(mNet);
  rubyInitialized = true;
}

Fetcher::OngoingRequest * Fetcher::get(const QNetworkRequest & request, VALUE code)
{
  QNetworkReply * reply = manager->get(request);
  OngoingRequest & r = ongoingRequests[reply];
  r.reply = reply;
  r.code = code;
  return &r;
}

VALUE Fetcher::getWrapper(VALUE obj, VALUE str)
{
  Fetcher * f;
  Data_Get_Struct(obj,Fetcher,f);
  f->get(QNetworkRequest(QUrl(valueToQString(str))), rb_block_proc());
  return Qnil;
}

void Fetcher::replyFinished(QNetworkReply* r)
{
  QTextStream err(stderr);
  if(! ongoingRequests.contains(r)) {
    /// \tdexception Raise here ? Or just log ?
    err << "Unkown reply " << r << endl
	<< "\tfor URL" << r->url().toString() << endl;
    return;
  }
  err << "Reply " << r << endl
      << "\tfor URL" << r->url().toString() << endl;
  VALUE code = ongoingRequests[r].code;
  Result * res = new Result(r);
  VALUE ary = rb_ary_new();
  rb_ary_push(ary, res->wrapToRuby());
  RescueWrapper2Args<VALUE, VALUE>::wrapCall(rb_proc_call, code, ary);
}
