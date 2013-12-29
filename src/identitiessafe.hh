/**
    \file identitiessafe.hh
    This class is responsible for safely storing identities (login/passwords)
    Copyright 2013 by Vincent Fourmond

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

#ifndef __IDENTITIESSAFE_HH
#define __IDENTITIESSAFE_HH

#include <attributehash.hh>
class Collection;

class IdentitiesSafe {
public:
  virtual ~IdentitiesSafe();

  /// Gets the credentials for logging into the website relevant to
  /// the collection.
  ///
  /// For now, static, maybe rely on a singleton class later on?
  static AttributeHash getCredentialsForCollection(Collection * collection);

};

#endif
