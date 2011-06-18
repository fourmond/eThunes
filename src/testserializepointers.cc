/*
    .cc: The Plugin class
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

#include <headers.hh>
#include <serializable-pointers.hh>

class BaseClass : public Serializable {
public:
  QString stuff;

public:
  virtual QString typeName() const {
    return "base";
  };

  static BaseClass * createObject(const QString & name);


  virtual SerializationAccessor* serializationAccessor() {
    SerializationAccessor * acc = new SerializationAccessor(this);
    acc->addAttribute("stuff", 
                      new SerializationItemScalar<QString>(&stuff, true));
    return acc;
  };
};


class Child1 : public BaseClass {
public:
  Child1() {
    stuff = "bidule";
  };
  virtual QString typeName() const {
    return "child1";
  };
  
};

class Child2 : public BaseClass {
public:
  virtual QString typeName() const {
    return "child2";
  };
};

BaseClass * BaseClass::createObject(const QString & name) {
  if(name == "base") 
    return new BaseClass;
  if(name == "child1") 
    return new Child1;
  if(name == "child2") 
    return new Child2;
  return NULL;
}

void testSerializePointers(const QStringList &)
{
  BaseClass * b[4];
  b[0] = new Child1;
  b[0]->stuff = "one thing";
  b[1] = new BaseClass;
  b[1]->stuff = "another one";
  b[2] = new Child2;
  b[3] = new Child1;
  QString str;
  QXmlStreamWriter xOut(&str);
  xOut.setAutoFormatting(true);
  xOut.setAutoFormattingIndent(2);
  QTextStream o(stdout);

  for(int i = 0; i < sizeof(b)/sizeof(BaseClass *); i++) {
    SerializationItemPointer<BaseClass> t(&b[i]);
    t.writeXML("test", &xOut);
  }
  o << "Serialized: " << endl
    << str << endl;

  o << "Now trying to read..." << endl;
  // Now is fun !
  BaseClass * c[4];
  QXmlStreamReader xIn(str);
  while(! xIn.isStartElement() && ! xIn.atEnd())
    xIn.readNext();
  for(int i = 0; i < sizeof(b)/sizeof(BaseClass *); i++) {
    SerializationItemPointer<BaseClass> t(&c[i]);
    o << "Element: ? " << xIn.name().toString() << endl;
    t.readXML(&xIn);
  }
}
