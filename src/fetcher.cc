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
  rb_define_method(cFetcher, "dummy_get", 
		   (VALUE (*)(...)) dummyGetWrapper, 1);


  rubyInitialized = true;
}

void Fetcher::dummyGet(const QString & url)
{
  QNetworkRequest request(url);
  QNetworkReply * reply = manager->get(request);
  QTextStream o(stdout);
  o << endl << "Fetching url : " << url << endl << endl;
  o << "Reply is " << reply << endl;
  // /// \todo Correct this, since the query will be finished *before*
  // /// all data has been pulled ;-)...
  // while(!reply->isFinished()) {
  //   QCoreApplication::processEvents();
  //   if(reply->canReadLine())
  //     o <<reply->readLine();
  //   sleep(1);
  // }
  // This approach does not work at all...
}

VALUE Fetcher::dummyGetWrapper(VALUE obj, VALUE str)
{
  Fetcher * f;
  Data_Get_Struct(obj,Fetcher,f);
  f->dummyGet(valueToQString(str));
  return Qnil;
}

void Fetcher::replyFinished(QNetworkReply* r)
{
  QTextStream o(stdout);
  o << endl << "Done fecthing : " << endl;
  o << r->readAll();
}
