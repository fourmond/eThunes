/*
    htlabel.cc: Implementation of HTLabel
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

#include <headers.hh>
#include <htlabel.hh>

HTLabel::HTLabel() : QLabel() {
  connect(this, SIGNAL(linkActivated(const QString &)),
          SLOT(onLinkClicked(const QString &)));
}

HTLabel::HTLabel(const QString & txt) : QLabel() {
  connect(this, SIGNAL(linkActivated(const QString &)),
          SLOT(onLinkClicked(const QString &)));
  setText(txt);
}




HTLabel::~HTLabel()
{
  freeTargets();
}

void HTLabel::freeTargets()
{
  for(int i = 0; i < ownedTargets.size(); i++)
    delete ownedTargets[i];
  ownedTargets.clear();
}


/// @todo This should probably move to HTTarget
HTTarget * HTLabel::targetFromUrl(const QString & url)
{
  if(url.startsWith("ht:")) {
    QString ptr = url.split(":")[1];
    return HTTarget::decodePointer(ptr);
  }
  return NULL;
}

void HTLabel::onLinkClicked(const QString & url)
{
  HTTarget * t = targetFromUrl(url);
  if(t)
    t->followLink();
  else {
    /// @todo Signal missing links ?
  }
}

void HTLabel::setText(const QString & str)
{
  freeTargets();
  QRegExp re("href\\s*=\\s*['\"]([^'\"]+)['\"]"); 
  int pos = 0;
  while ((pos = re.indexIn(str, pos)) != -1) {
    HTTarget * t = targetFromUrl(re.cap(1));
    if(t && t->isDisposable())
      ownedTargets << t;
    pos += re.matchedLength();
  }
  QLabel::setText(str);
}

