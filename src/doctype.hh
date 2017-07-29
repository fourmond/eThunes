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

class Collection;

/// This class defines the type of a document.
class DocType : public QObject {
  Q_OBJECT;

  /// The correspondance name -> DocType, in particular for
  /// inheritance purposes.
  static QHash<QString, DocType*> namedTypes;

  static QQmlEngine * engine;

  Q_PROPERTY(QString name READ name WRITE setName)

  /// The name of the type 
  QString m_Name;


  Q_PROPERTY(QString description READ description WRITE setDescription)
  
  /// A string describing the nature of the document type
  QString m_Description;


  Q_PROPERTY(QString infoFormat READ infoFormat WRITE setInfoFormat)
  
  /// A string giving a AttributeHash format for displaying a small
  /// informative text about a document of this type.
  QString m_InfoFormat;

  Q_PROPERTY(QStringList dates READ dateFields WRITE setDateFields)

  /// The fields that should be taken as dates
  QStringList m_Dates;

  Q_PROPERTY(QStringList amounts READ amountFields WRITE setAmountFields)

  /// The fields that should be taken as amounts
  QStringList m_Amounts;

  Q_PROPERTY(QStringList strings READ stringFields WRITE setStringFields)

  /// The fields that should be taken as pure strings
  QStringList m_Strings;


  Q_PROPERTY(QString parent READ parentName WRITE setParentName)

  /// The name of the parent
  QString m_ParentName;

  /// The parent document.
  DocType * parent;

  friend class Collection;

  /// The underlying collection.
  Collection * collection;

public:
  DocType(QObject * parent = NULL);
  virtual ~DocType();


  QString name() const;
  void setName(const QString & name);

  QString description() const;
  void setDescription(const QString & name);

  QString infoFormat() const;
  void setInfoFormat(const QString & name);

  QStringList dateFields() const;
  void setDateFields(const QStringList & df);

  QStringList stringFields() const;
  void setStringFields(const QStringList & df);

  QStringList amountFields() const;
  void setAmountFields(const QStringList & df);

  QString parentName() const;
  void setParentName(const QString & name);

  static void registerQMLTypes();
  static void parseQMLFile(const QString & file);

  /// Loops through all the types and make sure they all have a
  /// parent.
  static void crosslinkTypes();

  static DocType * namedType(const QString & name);
};

/// This class collects a series of DocType. It has no real purpose,
/// except for gathering the DocType objects and ensuring that they
/// have a well-defined provenance.
class Collection : public QObject {
  Q_OBJECT;

  Q_PROPERTY(QString name READ name WRITE setName)

  /// The name of the type 
  QString m_Name;


  Q_PROPERTY(QString description READ description WRITE setDescription)
  
  /// A string describing the nature of the document type
  QString m_Description;



  Q_PROPERTY(QQmlListProperty<DocType> docTypes READ getDocTypes)

  /// The document types held by this collection
  QList<DocType*> docTypes;

  Q_CLASSINFO("DefaultProperty", "docTypes")
  
protected:
  QQmlListProperty<DocType> getDocTypes();
  
  
public:

  Collection(QObject * parent = NULL);
  virtual ~Collection();


  QString name() const;
  void setName(const QString & name);

  QString description() const;
  void setDescription(const QString & name);

};





#endif
