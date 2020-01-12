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
class CarEvent : public Linkable {

public:

  enum Type {
    Purchase,
    Refuel,
    Repair,
    Other
  };

  /// The type of the event
  Type type;

  /// The kilometers at the event (-1 when unknown)
  int kilometers;

  /// The amount of fuel (in 0.01 liters), -1 if unkown
  int fuel;

  /// Whether the refuel corresponds to a full tank
  bool fullTank;

  /// The price per liter, -1 if unknown
  int pricePerLiter;

  virtual SerializationAccessor * serializationAccessor() override;
  
  virtual void followLink() override;

  virtual QString typeName() const override;
};

/// This class represents a car
class Car : public Serializable {
public:

  /// A public, user-defined name.
  QString name;
  
  QList<CarEvent> events;

  virtual SerializationAccessor * serializationAccessor() override;
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

  /// A list of cars...
  QList<Car> cars;

  virtual NavigationPage * pageForPlugin();

  virtual SerializationAccessor * serializationAccessor();

  virtual void finishedSerializationRead();

  CarPlugin();

  virtual bool hasBalance() const;

  virtual int balance() const;


};


#endif
