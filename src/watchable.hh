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

  friend class Watchable;


  /// Private constructor so only Watchable can build one.
  Watchdog();

  /// A correspondance watched objects watchdogs -> attribute name
  QHash<const Watchdog*, QString> watchedChildren;

  /// Watch a child
  void watchChild(const Watchable* child, const QString & attrName);

  /// Stop Watchin a child
  void unwatchChild(const Watchable* child);

signals:
  /// Emitted when one of the attributes of the watched target
  /// changed.
  void attributeChanged(const Watchdog * source, const QString & name);

  /// Emitted when one of the direct children has changed attribute
  void childChanged(const Watchdog * child, const QString & name, 
                    const QString & orig);

  /// Emitted when the number of items (if the object is a list) changed
  void numberChanged(const Watchdog * source);

  /// Emitted whenever the object changed.
  void changed(const Watchdog * source);

  /// Emitted whenever an object is inserted
  void objectInserted(const Watchdog * source, int at, int nb = 1);

  /// Emitted whenever an object is removed
  void objectRemoved(const Watchdog * source, int at, int nb = 1);

private slots:
  
  /// Catch any change in objects underneath
  void catchChange(const Watchdog * source);

public:
  /// If set to true, disable the forwarding of signals. Used for
  /// loading.
  static bool disableWatching;
};

/// This is the base class for all classes that emit signals when they
/// get modified.
class Watchable {
  /// The watchdog responsible for sending signals. Only created
  /// on-demand.
  mutable QSharedPointer<Watchdog> watchdog;

public:

  Watchable() : watchdog(new Watchdog()) {;};

  /// Used to obtain the watchdog for that, or create one if
  /// necessary.
  Watchdog * watchDog() const;

  /// This cast can be used directly to use connect with a Watchable
  /// object.
  operator const QObject*() const { return watchDog();};

  virtual ~Watchable();

protected:
  
  /// Sends message through the watchdog that an attribute has
  /// changed.
  void attributeChanged(const char * name) {
    watchdog->attributeChanged(watchDog(), name);
  }

  /// Sends message through the watchdog that the number of elements
  /// has changed.
  void numberChanged() {
    watchdog->numberChanged(watchDog());
  }


  /// Sends message through the watchdog that objects were inserted
  void objectInserted(int at, int nb = 1) {
    watchdog->objectInserted(watchDog(), at, nb);
  };

  /// Sends message through the watchdog that objects were removed
  void objectRemoved(int at, int nb = 1) {
    watchdog->objectRemoved(watchDog(), at, nb);
  }

  /// A helper function to 
  template<typename T> void setAttribute(T & dest, const T& source, 
                                         const char * name =  "") {
    if(dest == source)
      return;
    dest = source;
    attributeChanged(name);
  };

  /// Setup watching a child
  void watchChild(const Watchable* child, const QString & attrName) {
    watchdog->watchChild(child, attrName);
  };

  /// Stop watching a child
  void unwatchChild(const Watchable* child) {
    watchdog->unwatchChild(child);
  };


};

#endif
