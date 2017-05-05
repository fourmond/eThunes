/**
    \file periodic.hh
    The Periodic class, together with the Period
    Copyright 2017 by Vincent Fourmond

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


#ifndef __PERIODIC_HH
#define __PERIODIC_HH


/// A period is just two dates: a beginning and an end. The range is
/// inclusive.
class Period {
public:
  /// Beginning of the period
  QDate startDate;

  /// End of the period
  QDate endDate;

  /// Returns true if the given date is within the period
  bool isInside(const QDate & date) const;

  /// Whether the period is valid, i.e. both dates are valid and the
  /// endDate is after the beginning
  bool isValid() const;

  /// Whether this period @b starts @b before @a other
  bool operator<(const Period & other) const;

  /// Whether two periods are equal
  bool operator==(const Period & other) const;
};

/// The hash function, so that a Period can be used as the element of
/// a Hash. @relates Period
uint qHash(const Period & p, uint seed = 0);

/// This class represents a periodic event, i.e. a way to generate
/// Period objects that repeat.
///
/// For now, only monthly granularity.
class Periodic {
  /// The number of months
  int months;

  /// First month of the first period in the year (1 for January)
  int firstMonth;

public:
  Periodic(int m = 1, int fm = 1);

  /// Returns the period that contains the given date
  Period periodForDate(const QDate & date) const;

  /// Return the next period. Assumes the given Period was either
  /// given by a call to periodForDate() or nextPeriod().
  Period nextPeriod(const Period & period) const;

  /// Name the given period.
  QString periodName(const Period & period, bool longName = false) const;


  /// @name Predefined periodicities
  ///
  /// @{
  
  /// Monthly
  static const Periodic monthly;

  /// By trimester
  static const Periodic trimester;

  /// Yearly
  static const Periodic yearly;

  /// Schoolyear (starts in September, ends in August)
  static const Periodic schoolYear;

  /// @}
};




#endif
