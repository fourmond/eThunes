/**
    \file doctype.hh
    The DocType class, with related classes
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


#ifndef __DOCTYPE_HH
#define __DOCTYPE_HH


/// This class defines
class DocType : public QObject {
  Q_OBJECT;

  Q_PROPERTY(QString name READ name WRITE setName)

  QString m_Name;
public:
  DocType(QObject * parent = NULL);
  virtual ~DocType();


  QString name() const;
  void setName(const QString & name);

  static void registerQMLTypes();
  static void parseQMLFile(const QString & file);
};





#endif
