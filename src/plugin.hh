/**
    \file plugin.hh
    Base class for plugins doing their own business and keeping their
    data
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


#ifndef __PLUGIN_HH
#define __PLUGIN_HH

#include <serializable.hh>
#include <httarget.hh>

class NavigationPage;

class Plugin;
class Cabinet;


/// A class containing a plugin definition, ie a way to create a
/// Plugin, associated with a name, a public name and a description.
///
/// @todo Author information problably should come in later on.
class PluginDefinition {
public:

  /// A plugin creation function. Takes the
  typedef Plugin * (*Creator) (const QString &);

  /// Internal Plugin name
  QString name;

  /// Public name
  QString publicName;

  /// Description
  QString description;

  /// The function to create the plugin
  Creator creator;

  /// Creates a new plugin.
  Plugin * createPlugin() const;

  PluginDefinition(Creator c,
                   const QString & pub, const QString & desc,
                   const QString & name = "", 
                   bool autoRegister = true);
};


/// A plugin factory, managing a translation between name and Plugin *
/// creator.
class PluginFactory {
  QHash<QString, PluginDefinition *> registeredPlugins;

public:
  /// Creates a plugin from the name.
  Plugin * createNamedPlugin(const QString & name);

  /// Register the given plugin creation function.
  void registerPlugin(const QString & name, PluginDefinition * def);

  /// Returns the list of available plugins.
  QList<const PluginDefinition *> availablePlugins() const;
};

/// A Plugin is basically anything that would make sense in eThunes
/// from close or far, but that isn't specifically part of the core.
///
/// A Plugin keeps its own data, but is welcome to link to other bits.
///
/// Plugin keeps a list of available plugins, associated with a
/// code-like name.
///
/// @todo Ideally, a subclass of this one will handle plugins written
/// in Ruby code. That will be fun.
///
/// @todo This class will eventually have to be able to handle reading
/// raw XML and writing it back on serialization read/write as a
/// fallback in case a Cabinet was saved with a plugin but is read
/// without. This could be based on linking the QXMLStreamReader to a
/// QXMLStreamWriter to a QBuffer.
///
/// @todo I think the best thing is to provide a relatively type-safe
/// way to serialize a pointer and its data. In real, there are two
/// challenges here:
/// 
/// \li Second, I need a fallback in case the target type isn't
/// available, which greatly complexifies the first problem.
///
/// @todo I should rather save the information as separate XML
/// file. This would simply remove all difficulties, at the cost of a
/// slightly more complex layout. Maybe save as
/// plugin-data/{plugin}/name.xml ? (not that easy too, unless we
/// integrate directly as a subclass of SerializableAttribute)
class Plugin : public Serializable, public HTTarget {
private:
  /// The plugin creation factory.
  static PluginFactory * factory;

protected:

  /// A user-set name for the Plugin
  QString name;

public:

  /// The Cabinet the plugin is in.
  Cabinet * cabinet;

  Plugin() : cabinet(NULL) {;};


  /// The type name
  virtual QString typeName() const {
    return "(null)";
  };

  /// Returns the user-given name for the plugin.
  QString getName() const {
    return name;
  };

  /// Sets the user-given name for the plugin.
  void setName(const QString & n) {
    setAttribute(name, n, "name");
  };


  /// Returns a NavigationPage suitable to interact with the Plugin.
  ///
  /// @todo Maybe we could add a QWidget/QDialog too ?
  virtual NavigationPage * pageForPlugin() = 0;

  /// For serialization
  virtual SerializationAccessor * serializationAccessor();

  /// The plugin system is somewhat disconnected from the
  /// serialization system, in that the plugins are not required to
  /// use XML for saving/loading. Instead, they rely on string
  /// functions, that may in turn use the serialization functions
  /// again or use a completely different mechanism (YAML could be
  /// used for ruby, for instance).
  ///
  /// The default implementation is to wrap the object in a XML stream
  /// again, and save it as CDATA.
  virtual QString writeAsString();
  virtual void readFromString(const QString & str);

  virtual ~Plugin();

  /// Creates a plugin from the name.
  ///
  /// It creates a MissingPlugin instance should the type be
  /// unavailable at run time.
  static Plugin * createNamedPlugin(const QString & name);

  static Plugin * createObject(const QString & name) {
    return createNamedPlugin(name);
  };

  /// Register the given plugin creation function.
  static void registerPlugin(const QString & name, 
                             PluginDefinition * def);

  static QList<const PluginDefinition *> availablePlugins();

  virtual void followLink();

  /// @name Balance-based functions
  ///
  /// Plugins may refer to assets (or debts) in themselves, and this
  /// can be retrieved using the functions here.
  /// @{


  /// Whether or not the plugin has a balance (defaults to false)
  virtual bool hasBalance() const;
  
  /// The plugins current balance
  virtual int balance() const;

  /// A transaction list that essentially tells the balance over time
  //virtual QList<

  /// @}
};


#endif
