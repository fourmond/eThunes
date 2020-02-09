/**
   \file car.hh
   Plugin to keep track of cars
   Copyright 2020 by Vincent Fourmond

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


#ifndef __CAR_HH
#define __CAR_HH

#include <plugin.hh>
#include <linkable.hh>
#include <transactionlists.hh>
#include <watchablecontainers.hh>
#include <evolvingitem.hh>

#include <categorizable.hh>

class CarPlugin;

/// An event is one of:
/// @li purchase/sale of a car
/// @li refuel
/// @li repair
/// @li other expenses
/// Repairs and other expenses can be tagged with something saying
/// they can repeat every that many kilometers/that much time/whatever
/// comes first.
///
/// An even is linked to a single transaction.
class CarEvent : public Linkable, public Categorizable {
protected:

  /// Returns the transaction that defines this event.
  AtomicTransaction * underlyingTransaction() const;

public:

  CarPlugin * plugin;

  enum Type {
    Purchase,
    Refuel,
    Repair,
    Toll,
    Other
  };

  /// @name Attributes
  ///
  /// @{
  
  /// The type of the event
  Type type;

  /// The kilometers at the event (-1 when unknown)
  int kilometers;

  /// The amount of fuel (in 0.01 liters), -1 if unkown
  int fuel;

  /// Whether the refuel corresponds to a full tank
  bool fullTank;

  /// The price per liter, -1 if unknown.
  /// Hmmm. Should perhaps have a better resolution ?
  int pricePerLiter;

  /// @}

  /// @name Cache attributes
  ///
  /// These attributes are set by the Car object, they are used for
  /// caching.
  ///
  /// @{

  /// The real kilometers or interpolated kilometers.
  int interpolatedKilometers;

  /// @}
  
  CarEvent();

  /// @name Accessors
  ///
  /// @{
  
  /// Returns the date of the underlying transaction
  QDate date() const;

  /// Returns the amount of the underlying transaction
  int amount() const;

  /// Returns the price per liter, and set the deduce flag if it was
  /// computed from the rest. -1 means unkown.
  int fuelPrice(bool * computed) const;

  /// Returns the number of liters, and set the deduce flag if it was
  /// computed from the rest. -1 means unkown
  int fuelLiters(bool * computed) const;

  /// @}

  /// Sorts by kilometers if both are available or by date otherwise.
  /// Hmmm. Sorts just by date for now.
  bool operator<(const CarEvent & other) const;
  

  virtual SerializationAccessor * serializationAccessor() override;
  virtual void followLink() override;
  virtual QString typeName() const override;
};

/// This class represents a car
class Car : public Serializable {
  QList<int> totals;
  int total;
  int liters;

  int lastkm;

  /// Last events with the given tag
  QHash<const Tag *, int> lastTaggedEvent;
  
  friend class CarPage;
public:

  Car();

  /// The amount of emitted CO2 in grams
  int fuelCO2Emission;

  /// The base plugin.
  CarPlugin * plugin;

  /// The default price for the fuel
  EvolvingItem<int> defaultPricePerLiter;
  
  // /// A public, user-defined name.
  // QString name;
  
  WatchableList<CarEvent> events;

  virtual SerializationAccessor * serializationAccessor() override;
  // virtual void finishedSerializationRead() override;

  void addEvents(const TransactionPtrList & events, CarEvent::Type type);

  /// Updates the cache
  void updateCache();

  /// The total number of kilometers so far.
  int kilometers() const;
};


/// This plugin helps to keep track of the cars one have.
///
/// @todo Maybe there should be a way for a plugin to add an "item" in
/// the "balance" panel of the Cabinet ?
class CarPlugin : public Plugin {
public:
  virtual QString typeName() const {
    return "car";
  };

  /// Just use a single car for now.
  Car car;

  virtual NavigationPage * pageForPlugin();
  virtual SerializationAccessor * serializationAccessor();
  virtual void finishedSerializationRead() override;

  virtual QList<QPair<QString, TransactionPtrList::Action> > transactionContextMenu();
  

  CarPlugin();


};


#endif
