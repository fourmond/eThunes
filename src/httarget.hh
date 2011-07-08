/**
    \file httarget.hh
    Base class for objects that can be the target of hypertextlinks
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


#ifndef __HTTARGET_HH
#define __HTTARGET_HH

/// This class is the base class for all objects that can be embedded
/// as links in a QLabel (or, more likely, in an appropriate derived
/// class), such as:
/// \li true Linkable objects (which should turn into a
/// subclass of this one)
/// \li one-shot functions (with templates)
class HTTarget {
public:
  
  /// Follow the link, ie perform the action connected to the
  /// link. This can be anything:
  /// \li goto an appropriate NavigationPage
  /// \li popup a dialog box
  /// \li even popup directly a QLineEdit or something like this
  /// \li crash the computer
  virtual void followLink() = 0;

  /// Whether the Label displaying the link should take ownership of
  /// the object or not
  virtual bool isDisposable() = 0;

  /// Performs what happens when the link is hovered. Default action
  /// is to show a tooltip containing toolTip();
  virtual void hoverLink();

  /// Returns a toolTip to be used with the default hoverLink()
  /// action.
  virtual QString toolTip();


  /// Encodes the given pointer as a string
  static QString encodePointer(HTTarget * t);

  /// Decodes the given pointer from a string.
  static HTTarget * decodePointer(const QString & str);
  
};

#endif
