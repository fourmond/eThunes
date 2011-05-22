/**
    \file watchable.hh
    Infrastructure for signalling for objects that do not descend from QObject
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


#ifndef __WATCHABLE_HH
#define __WATCHABLE_HH

class Watchable;

/// This is a helper class that sends the signals using Qt's
/// signalling mechanism.
class Watchdog : public QObject {
  Q_OBJECT;

  /// The target
  const Watchable * target;

  friend class Watchable;


  /// Private constructor so only Watchable can build one.
  Watchdog(const Watchable * w) : target(w) {;};

signals:
  /// Emitted when one of the attributes of the watched target
  /// changed.
  void attributeChanged(const QString & name, const Watchable * source);

  /// Emitted when the number of items (if the object is a list) changed
  void numberChanged(const Watchable * source);
};

/// This is the base class for all classes that 
class Watchable {
  /// The watchdog responsible for sending signals. Only created
  /// on-demand.
  mutable Watchdog * watchdog;

public:

  Watchable() : watchdog(NULL) {;};
  Watchable(const Watchable & ) : watchdog(NULL) { ;  };

  /// Used to obtain the watchdog for that, or create one if
  /// necessary.
  const Watchdog * watchDog() const;

  /// This cast can be used directly to use connect with a Watchable
  /// object.
  operator const QObject*() const { return watchDog();};

  virtual ~Watchable();

protected:
  
  void attributeChanged(const QString & name = QString());

  template<typename T> void setAttribute(T & dest, const T& source, const QString & name = "") {
    if(dest == source)
      return;
    dest = source;
    attributeChanged(name);
  };
};

#endif
