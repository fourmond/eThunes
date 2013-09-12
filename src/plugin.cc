/*
    plugin.cc: The Plugin class
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
#include <plugin.hh>
#include <navigationwidget.hh>


PluginDefinition::PluginDefinition(Creator c,
                                   const QString & pub, const QString & desc,
                                   const QString & n, 
                                   bool autoRegister) :
  publicName(pub), description(desc), creator(c)
{
  if(n.isEmpty()) {
    Plugin * p = c("");
    name = p->typeName();
    delete p;
  }
  else
    name = n;
  if(autoRegister)
    Plugin::registerPlugin(name, this);
}

Plugin * PluginDefinition::createPlugin() const
{
  return creator(name);
}

//////////////////////////////////////////////////////////////////////

void PluginFactory::registerPlugin(const QString & name, 
                                   PluginDefinition * def)
{
  if(registeredPlugins.contains(name))
    /// @todo That isn't too beautiful too...
    throw "Attempting to register twice the same plugin";
  registeredPlugins[name] = def;
}

Plugin * PluginFactory::createNamedPlugin(const QString & name)
{
  if(registeredPlugins.contains(name))
    return registeredPlugins[name]->createPlugin();
  return NULL;
}


QList<const PluginDefinition *> PluginFactory::availablePlugins() const
{
    QList<const PluginDefinition *> retval;
    QHash<QString, PluginDefinition*>::const_iterator i = 
      registeredPlugins.constBegin();
    while(i != registeredPlugins.constEnd()) {
      retval.append(i.value());
      i++;
    }
    return retval;
}


PluginFactory * Plugin::factory = NULL;


//////////////////////////////////////////////////////////////////////


/// This class represents a missing plugin, ie a placeholder to make
/// sure plugin data is not lost when a plugin is missing. It doesn't
/// do anything else.
class MissingPlugin : public Plugin {
protected:

  /// Plugin data, read at load time.
  QString originalData;

  /// Plugin original type
  QString originalType;

public:

  MissingPlugin(const QString & type) : originalType(type) {
    ;
  };

  virtual QString typeName() const {
    return originalType;
  };
  
  virtual QString writeAsString() {
    return originalData;
  };

  virtual void readFromString(const QString & str) {
    originalData = str;
  };

  virtual NavigationPage * pageForPlugin() {
    return NULL;                // Probably not a good idea.
  };

};


//////////////////////////////////////////////////////////////////////


Plugin * Plugin::createNamedPlugin(const QString & name)
{
  Plugin * plugin = NULL;
  if(factory)
    plugin = factory->createNamedPlugin(name);
  if(! plugin)                  /// @todo Log that ?
    plugin = new MissingPlugin(name);
  return plugin;
}

void Plugin::registerPlugin(const QString & name, 
                            PluginDefinition * def)
{
  if(! factory)
    factory = new PluginFactory;
  factory->registerPlugin(name, def);
}

SerializationAccessor * Plugin::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name);
  return ac;
}

Plugin::~Plugin()
{
}

QList<const PluginDefinition *> Plugin::availablePlugins()
{
  if(factory)
    return factory->availablePlugins();
  return QList<const PluginDefinition *>();
}

void Plugin::followLink() 
{
  NavigationPage * page = pageForPlugin();
  if(page)
    NavigationWidget::gotoPage(page);
}

bool Plugin::hasBalance() const
{
  return false;
}

int Plugin::balance() const
{
  return 0;
}

QString Plugin::writeAsString()
{
  QString str;
  QXmlStreamWriter xOut(&str);
  xOut.setAutoFormatting(true);
  xOut.setAutoFormattingIndent(2);
  writeXML("data", &xOut);
  return str;
}

void Plugin::readFromString(const QString & str)
{
  QXmlStreamReader xIn(str);
  Serialization::readNextToken(&xIn);
  while(! ((xIn.isStartElement() && xIn.name() == "data") || 
           xIn.atEnd()))
    Serialization::readNextToken(&xIn);
  readXML(&xIn);

}

