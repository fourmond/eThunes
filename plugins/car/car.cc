/*
  car.cc: implementation of the Car plugin
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

#include <headers.hh>
#include <utils.hh>
#include <transaction.hh>

#include "car.hh"
#include "carpage.hh"

// #include <httarget-templates.hh>

#include <pointersafesort.hh>

CarEvent::CarEvent() :
  type(Other),
  kilometers(-1),
  fuel(-1),
  fullTank(false),
  pricePerLiter(-1),
  interpolatedKilometers(-1),
  totalAmount(0)
{
}


SerializationAccessor * CarEvent::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  addLinkAttributes(ac);
  ac->addScalarAttribute("type", (int*)&type);
  ac->addScalarAttribute("kilometers", &kilometers);
  ac->addScalarAttribute("fuel", &fuel);
  ac->addScalarAttribute("full-trank", &fullTank);
  ac->addScalarAttribute("price-per-liter", &pricePerLiter);
  return ac;
}

void CarEvent::followLink()
{
  // TODO
}

QString CarEvent::typeName() const
{
  return "car-event";
}

AtomicTransaction * CarEvent::underlyingTransaction() const
{
  QList<AtomicTransaction*> trs =
    links.typedLinks<AtomicTransaction>("car-event");
  return trs.value(0, NULL);
}

QDate CarEvent::date() const
{
  AtomicTransaction * t = underlyingTransaction();
  if(t)
    return t->getDate();
  return QDate();
}

int CarEvent::amount() const
{
  AtomicTransaction * t = underlyingTransaction();
  if(t)
    return -t->getAmount();     // We count expenses as positive
  return 0;
}

int CarEvent::fuelPrice(bool * computed) const
{
  if(type != Refuel)
    return -1;
  *computed = false;
  if(pricePerLiter >= 0)
    return pricePerLiter;
  *computed = true;
  if(fuel >= 0)
    return amount()*100/fuel;
  return -1;
}

int CarEvent::fuelLiters(bool * computed) const
{
  if(type != Refuel)
    return -1;
  *computed = false;
  if(fuel >= 0)
    return fuel;
  *computed = true;
  if(pricePerLiter >= 0)
    return amount()*100/pricePerLiter;
  return -1;
}


bool CarEvent::operator<(const CarEvent & other) const
{
  return date() < other.date();
}



//////////////////////////////////////////////////////////////////////

SerializationAccessor * Car::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  // ac->addScalarAttribute("name", &name);
  ac->addListAttribute("event", &events);
  return ac;
}

/// We need that to make g++ find the correct swap function.
void qSwap(MyQListIterator<CarEvent>::PointedTo a, 
           MyQListIterator<CarEvent>::PointedTo b) 
{
  MyQListIterator<CarEvent>::swap(a,b);
}

void Car::addEvents(const TransactionPtrList & transactions,
                    CarEvent::Type type)
{
  int nb = 0;
  for(AtomicTransaction * t : transactions) {
    if(! t->hasNamedLinks("car-event")) {
      events << CarEvent();
      CarEvent & ev = events.last();
      ev.type = type;
      ev.plugin = plugin;
      ev.addLink(t, "car-event");
      ++nb;
    }
  }
  if(nb > 0) {
    pointerSafeSortList(&events.rawData());
    updateCache();
  }
}

void Car::updateCache()
{
  // Assumes the events are sorted by date.
  int total = 0;
  int lastkmpos = -1;
  for(int i = 0; i < events.size(); i++) {
    total += events[i].amount();
    events[i].totalAmount = total;
    events[i].interpolatedKilometers = -1;
    if(events[i].kilometers > 0) {
      if(lastkmpos >= 0) {
        QDate old = events[lastkmpos].date();
        int lastkm = events[lastkmpos].kilometers;
        QDate cur = events[i].date();
        int km = events[i].kilometers;
        for(int k = lastkmpos+1; k < i; k++) {
          events[k].interpolatedKilometers =
            lastkm + (km - lastkm) * (old.daysTo(events[k].date()))/
            (1.0*old.daysTo(cur));
        }
      }
      lastkmpos = i;
    }
  }
}

//////////////////////////////////////////////////////////////////////

CarPlugin::CarPlugin()
{
}

SerializationAccessor * CarPlugin::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name); // MUST NOT BE FORGOTTEN !
  ac->addAttribute("car", &car);
  return ac;
}

NavigationPage * CarPlugin::pageForPlugin()
{
  return CarPage::getCarPage(this);
}


QList<QPair<QString, TransactionPtrList::Action> > CarPlugin::transactionContextMenu()
{
  QList<QPair<QString, TransactionPtrList::Action> > actions;
  Car * c = &car;

  // Adds one action
  auto f = [c, &actions](const QString & name, CarEvent::Type type) {
    TransactionPtrList::Action act = [c, type](const TransactionPtrList & list) -> void {
      c->addEvents(list, type);
    };
    actions << QPair<QString, TransactionPtrList::Action>(name, act);
  };
  f("Purchase", CarEvent::Purchase);
  f("Fuel", CarEvent::Refuel);
  f("Repair", CarEvent::Repair);
  f("Other", CarEvent::Other);
  return actions;
}


//////////////////////////////////////////////////////////////////////


static Plugin * carCreator(const QString &)
{
  return new CarPlugin();
}

static PluginDefinition car(carCreator, 
                            "Car tracker",
                            "");
