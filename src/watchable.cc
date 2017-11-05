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


/// @todo I should add a QSet<Watchable *> attribute to the Watchdog:
/// each time a new Watchable is created, either by copy constructor
/// or directly, it adds itself there. Upon deletion, it removes
/// itself. It would be an error to have more than two at the same
/// time (?), and an error too to have more than one when a slot is
/// processed.
///
/// @todo make something intersting with childChanged
Watchdog::Watchdog() {
  connect(this, SIGNAL(attributeChanged(const Watchdog *, const QString &)),
          SIGNAL(changed(const Watchdog *)));
  connect(this, SIGNAL(numberChanged(const Watchdog *)),
          SIGNAL(changed(const Watchdog *)));
}

void Watchdog::catchChange(const Watchdog * source)
{
  QHash<const Watchdog*, QString>::const_iterator i = 
    watchedChildren.find(source);
  // QTextStream o(stdout);
  // o << "catchChange for " << this
  //   << "(" << typeid(this).name() << ") from " << source
  //   << "(" << typeid(source).name() << ")"
  //   << endl;
  if(i != watchedChildren.end()) {
    emit(attributeChanged(this, i.value()));
    // o << " -> " << i.value() << endl;
  }
}

void Watchdog::watchChild(const Watchable* child, 
                          const QString & attrName)
{
  watchedChildren[child->watchDog()] = attrName;
  connect(child->watchDog(), SIGNAL(changed(const Watchdog *)),
          SLOT(catchChange(const Watchdog *)));
}

void Watchdog::unwatchChild(const Watchable* child)
{
  if(watchedChildren.remove(child->watchDog()))
    child->watchDog()->disconnect(this);
}


Watchdog * Watchable::watchDog() const
{
  return watchdog.data();
}

Watchable::~Watchable()
{
  // delete watchdog;
}
