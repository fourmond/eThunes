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

#include <attributehash.hh>

class Collection;
class Period;
class AtomicTransaction;
class Document;

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


  /// Attributes for date matching.
  /// @todo Should be settable from the QML source
  int daysBefore;
  int daysAfter;



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
  QStringList allDateFields() const;
  void setDateFields(const QStringList & df);

  QStringList stringFields() const;
  void setStringFields(const QStringList & df);

  QStringList amountFields() const;
  QStringList allAmountFields() const;
  void setAmountFields(const QStringList & df);

  QString parentName() const;
  void setParentName(const QString & name);


  /// Returns the attributes required by the DocType
  QHash<QString, AttributeHash::HandledType> requiredAttributes();

  static void registerQMLTypes();
  static void parseQMLFile(const QString & file);

  /// Goes through all the type definition files found on the system
  /// and load them up !
  static void loadTypes();

  /// Loops through all the types and make sure they all have a
  /// parent.
  static void crosslinkTypes();

  /// Returns the DocType corresponding to the (unique) name
  static DocType * namedType(const QString & name);

  /// Returns all the named types.
  static QStringList documentNames();

  /// Return all the documents types, categorized by their Collection
  static QHash<Collection *, QList<DocType *> > docTypesByCollection();

  /// @name Access to script functions
  ///
  /// The document types can optionally provide ways to automatically
  /// detect whether a document matches its type, and to automatically
  /// read the documents meta-data.
  ///
  /// @{
  
  /// Returns true if the DocType has a isMine function declaration.
  bool hasIsMine() const;

  /// Returns a certain number indicating whether the given document
  /// attributes (provided by PDFTools::readPDF, or any other source)
  /// could be the given type. The highest bid gets the type.
  ///
  /// Returns 0 if the document does not support detection.
  int isMine(const AttributeHash & contents);

  /// Returns true if the DocType has a parseMetaData function declaration.
  bool hasParseMetaData() const;

  /// Returns a certain number indicating whether the given document
  /// attributes (provided by PDFTools::readPDF, or any other source)
  /// could be the given type. The highest bid gets the type.
  ///
  /// Returns 0 if the document does not support detection.
  AttributeHash parseMetaData(const AttributeHash & contents);


  /// @}

  /// Attempts autodetection of the document type, by passing @a
  /// contents to isMine() of all the document types that have them.
  ///
  /// Returns the DocType found, or NULL if none was found.
  static DocType * autoDetectType(const AttributeHash & contents);



  /// @name Transaction-matching capacities
  ///
  /// These functions can be used to find an AtomicTransaction that
  /// matches a Document whose type is the DocType.
  ///
  /// @todo These could be implemented in JS, instead of using the
  /// default implementation.
  ///
  /// @{

  /// Returns the score for the match between the given transaction
  /// and the given document.
  ///
  /// The higher the score, the more likely the document corresponds.
  ///
  /// By default, the score will be non-zero only if the amount is
  /// right (in absolute value), and will decrease with the dinstance
  /// in time.
  int scoreForTransaction(const Document * doc, AtomicTransaction * tr) const;

  /// Returns the date range in which a matching transaction could be
  /// found.
  Period relevantDateRange(const Document * doc) const;
  /// @}

  /// @name Helper for DocType JS code
  ///
  /// A series of helper functions to parse documents in JS code.
  ///
  /// @{

  Q_PROPERTY(QString frenchMonthsRE READ getFrenchMonthRE)

  /// Returns a RE suitable to match french months
  QString getFrenchMonthRE() const;


  /// Parse a french month. Returns 0 for January, etc.
  Q_INVOKABLE int parseFrenchMonth(const QString & text);

  /// @}

};

/// This class collects a series of DocType. It has no real purpose,
/// except for gathering the DocType objects and ensuring that they
/// have a well-defined provenance.
class Collection : public QObject {
  Q_OBJECT;

  Q_PROPERTY(QString name READ name WRITE setName)

  /// The name of the collection
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

  /// @name Functions to deal with property list
  ///
  /// @{
  static void appendDocType(QQmlListProperty<DocType> *property, DocType *value);
  static DocType * atDocType(QQmlListProperty<DocType> *property, int index);
  static void clearDocType(QQmlListProperty<DocType> *property);
  static int countDocType(QQmlListProperty<DocType> *property);
  /// @}
  
public:

  Collection(QObject * parent = NULL);
  virtual ~Collection();


  QString name() const;
  void setName(const QString & name);

  QString description() const;
  void setDescription(const QString & name);

};





#endif
