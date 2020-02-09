/**
    \file carpage.hh
    The main page to interact with the Car plugin
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

#ifndef __CARPAGE_HH
#define __CARPAGE_HH

#include <navigationpage.hh>
#include "car.hh"

class HTLabel;

/// The main page of the Car plugin.
class CarPage : public NavigationPage {

  Q_OBJECT;

protected:

  static QHash<CarPlugin *, CarPage *> carPages;

  CarPlugin * plugin;

  /// The main summary (in the top ?)
  HTLabel * summary;

public:

  CarPage(CarPlugin * plugin);

  /// Returns the CollectionPage for the given collection, or create
  /// it if it doesn't exist yet.
  static CarPage * getCarPage(CarPlugin * plugin);

  /// Returns the title of the page
  virtual QString pageTitle();


public slots:

  /// Update the page.
  void updatePage();

  /// Change default price
  void editDefaultPrice();

  /// Change CO2 emissions
  void editCO2Emissions();

  // /// Shows the kilometers over time
  // void showKilometers();
};

#endif
