/** 
    \file result.hh
    The result of a Web query, a kind of (ruby) wrapper around QNetworkReply
    Copyright 2010 by Vincent Fourmond

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


#ifndef __RESULT_HH
#define __RESULT_HH

/// A class wrapping a QNetworkReply for Ruby
class Result {

protected:

  /// The reply
  QNetworkReply * reply;

  /// The data
  QByteArray data;

  /// The "Result" class
  static VALUE cResult;
  
  /// Whether the Ruby definitions of this class have been loaded.
  static bool rubyInitialized;

  /// The Ruby garbage collector.
  static void rubyFree(VALUE v);

  /// Accessor for the contents of the request's reply.
  static VALUE contentsAccessor(VALUE obj);

  /// Accessor for the raw headers
  static VALUE rawHeadersAccessor(VALUE obj);

  /// Accessor for the reply's URL (which may differ from the request
  /// URL !)
  static VALUE urlAccessor(VALUE obj);

public:

  /// Constructs a Result object from the given reply.
  ///
  /// \warning The reply must be finished, else the constructor will
  /// most probably hang forever waiting for data that will never be
  /// able to come !
  Result(QNetworkReply * r);

  /// Initializes the Ruby side of things; mNet is the Net module.
  static void initializeRuby(VALUE mNet);

  /// Returns a Ruby object wrapping this object.
  ///
  /// \warning initializeRuby() must have been called beforehand.
  VALUE wrapToRuby();
};
#endif
