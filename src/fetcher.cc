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
#include <ruby-templates.hh>
#include <fetcher.hh>
#include <result.hh>

#include <collection.hh>

#include <logstream.hh>


using namespace Ruby;

bool Fetcher::rubyInitialized = false;

VALUE Fetcher::mNet;

VALUE Fetcher::cFetcher;

Fetcher::Fetcher() : followRedirections(true),
		     targetWallet(0),
		     targetCollection(0)
{
  manager = new QNetworkAccessManager(this);
  cookieJar = new CookieJar();
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

  rb_define_method(cFetcher, "private_get",
		   (VALUE (*)(...)) getWrapper, 1);

  rb_define_method(cFetcher, "private_post",
		   (VALUE (*)(...)) postWrapper, 2);

  rb_define_method(cFetcher, "add_document",
		   (VALUE (*)(...)) addDocumentWrapper, 2);

  rb_define_method(cFetcher, "dump_cookies",
		   (VALUE (*)(...)) cookiesWrapper, 0);


  Result::initializeRuby(mNet);
  rubyInitialized = true;
}

Fetcher::OngoingRequest * Fetcher::get(const QNetworkRequest & request,
                                       int redirections)
{
  QNetworkReply * reply = manager->get(request);
  return registerRequest(reply, redirections);
}

Fetcher::OngoingRequest * Fetcher::registerRequest(QNetworkReply * reply,
						   int redirections)
{
  LogStream info(Log::Info);
  OngoingRequest & r = ongoingRequests[reply];
  r.reply = reply;
  info << "Requesting URL: " << reply->request().url().toString() 
       << " on behalf of " 
       << (targetCollection ? "collection " : "the wallet") 
       << (targetCollection ? targetCollection->name : "") 
       << endl;
    
  r.done = false;
  r.maxHops = (redirections ?  redirections : 7 /** \todo Customize this */);
  return &r;
}


void Fetcher::setFiber(VALUE f)
{
  fiber = f;
  Ruby::keepSafe(fiber);           // DO NOT GARBAGE COLLECT !
}

Fetcher::OngoingRequest * Fetcher::post(const QNetworkRequest & request,
					const AttributeHash & parameters)
{
  /// \todo This looks more like a dirty hack than a proper
  /// solution. Taken from:
  ///
  /// http://stackoverflow.com/questions/2214595/how-can-i-create-a-http-post-request-with-qt-4-6-1
  ///
  /// Although it seems to work with SFR, which is a good thing
  /// already.
  QByteArray data;
  QUrl params;
  QNetworkRequest re = request;
  AttributeHash::const_iterator i = parameters.constBegin();
  while (i != parameters.constEnd()) {
    params.addQueryItem(i.key(), i.value().toString());
    i++;
  }
  data.append(params.toString());
  data.remove(0,1);

  re.setHeader(QNetworkRequest::ContentTypeHeader,
	      "application/x-www-form-urlencoded");
  QNetworkReply * reply = manager->post(re, data);
  return registerRequest(reply);
}

VALUE Fetcher::cookiesWrapper(VALUE obj)
{
  Fetcher * f = fromValue(obj);
  if(f->cookieJar) {
    f->cookieJar->dumpContents();
  }
  return Qnil;
}


VALUE Fetcher::getWrapper(VALUE obj, VALUE str)
{
  Fetcher * f = fromValue(obj);
  f->get(QNetworkRequest(QUrl(valueToQString(str))));
  return Qnil;
}

VALUE Fetcher::postWrapper(VALUE obj, VALUE str, VALUE hash)
{
  Fetcher * f = fromValue(obj);
  AttributeHash val;
  f->post(QNetworkRequest(QUrl(valueToQString(str))),
	  AttributeHash::fromRuby(hash));
  return Qnil;
}

void Fetcher::replyFinished(QNetworkReply* r)
{
  LogStream err(Log::Error);
  LogStream info(Log::Info);
  if(! ongoingRequests.contains(r)) {
    /// \tdexception Raise here ? Or just log ?
    err << "Unkown reply " << r << endl
	<< "\tfor URL" << r->url().toString() << endl;
    return;
  }

  // err << "Reply " << r << endl
  //     << "\tfor URL" << r->url().toString() << endl;

  // // Dumping cookie jar:
  // QTextStream o(stdout);
  // /// \todo Customize the display of this ?
  // o << "Cookie jar:" << endl;
  // cookieJar->dumpContents();
  // cookieJar->dumpTree(o);
  if(followRedirections && r->hasRawHeader("location")) {
    // That looks very much like a redirection
    QUrl target = 
      r->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    info << " -> redirects to " << target.toString() << endl;
    int nb = ongoingRequests[r].maxHops - 1;
    if(nb)
      get(QNetworkRequest(target), nb);
    else
      err << "Maximum number of redirections hit, stopping" << endl;

  }
  else {
    Result * res = new Result(r);
    VALUE f = res->wrapToRuby();
    Ruby::wrappedFuncall(fiber, rb_intern("resume"), 1, f);
  }
  // In any case, the request is done.
  ongoingRequests[r].done = true;
  /// \todo Here, we should find a way to ask for the fetcher's
  /// destruction when all requests have been finished.

  int nbOn = 0;
  QHash<QNetworkReply *, OngoingRequest>::iterator i;
  for(i = ongoingRequests.begin(); i != ongoingRequests.end();
      i++)
    if(! i.value().done)
      nbOn++;
  if(nbOn == 0)
    emit(requestsFinished(this));
  
}

bool Fetcher::addDocument(Result * result, const QString & doctype)
{
  bool retval = false;
  if(targetCollection) {
    // Here, we save the document to a temporary file, and fire ahead
    QTemporaryFile * file = result->saveToTemporaryFile();
    retval = targetCollection->importFile(doctype, file->fileName());

    /// \todo When we will be using the poppler library to parse PDF
    /// files directly, rather than spawning pdftotext which is
    /// cumbersome and not that great, we should get rid of the
    /// temporary file.
    delete file;		// We dispose of the temporary file.
  }
  else if(targetWallet) {
    /// \todo...
  }
  else {
    LogStream o(Log::Error);
    o << "Should be fowarding a document from Fetcher (" << this
      << ") but there is no target" << endl;
  }
  return retval;
}

VALUE Fetcher::addDocumentWrapper(VALUE obj, VALUE result,
				  VALUE doctype)
{
  Fetcher * f = fromValue(obj);
  Result * r = Result::fromValue(result);
  f->addDocument(r, valueToQString(doctype));
  return Qtrue;
}
