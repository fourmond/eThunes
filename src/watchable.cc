/*
    watchable.cc: Infrastructure for signalling
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

#include <watchable.hh>


Watchdog::Watchdog(const Watchable * w) : target(w) {
  connect(this, SIGNAL(attributeChanged(const Watchable *, const QString &)),
          SIGNAL(changed(const Watchable *)));
}

void Watchdog::catchChange(const Watchable * source)
{
  QHash<const Watchable*, QString>::const_iterator i = 
    watchedChildren.find(source);
  if(i != watchedChildren.end())
    emit(attributeChanged(target, i.value()));
}

void Watchdog::watchChild(const Watchable* child, 
                          const QString & attrName)
{
  watchedChildren[child] = attrName;
  connect(child->watchDog(), SIGNAL(changed(const Watchable *)),
          SLOT(catchChange(const Watchable *)));
}


Watchdog * Watchable::watchDog() const
{
  if(! watchdog)
    watchdog = new Watchdog(this);
  return watchdog;
}

Watchable::~Watchable()
{
  delete watchdog;
}
