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

class NavigationPage;

class Plugin;

/// A plugin factory, managing a translation between name and Plugin *
/// creator.
class PluginFactory {
public:
  /// A creation function
  typedef Plugin * (*Creator) (const QString &);

private:
  QHash<QString, Creator> registeredPlugins;

public:
  /// Creates a plugin from the name.
  Plugin * createNamedPlugin(const QString & name);

  /// Register the given plugin creation function.
  void registerPlugin(const QString & name, Creator creator);
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
class Plugin : public Serializable {
private:
  /// @name Backup attributes
  ///
  /// These attributes are NOT FUNCTIONAL AS OF NOW !
  /// 
  /// @{
  /// The data for the plugin, saved as raw XML. This attribute is
  /// useless in derived classes and should be empty.
  QString savedData;

  /// The type name for the plugin, saved in case no derived class
  /// took it.
  QString savedType;
  //@}

  /// The plugin creation factory.
  static PluginFactory * factory;

protected:

  /// A user-set name for the Plugin
  QString name;

public:

  /// The type name
  virtual const QString & typeName() const {
    return savedType;
  };


  /// Returns a NavigationPage suitable to interact with the Plugin.
  ///
  /// @todo Maybe we could add a QWidget/QDialog too ?
  virtual NavigationPage * pageForPlugin();

  // /// For serialization
  // virtual SerializationAccessor * serializationAccessor();

  // // These functions need to be reimplemented to handle the case when...
  // virtual void writeXML(const QString & name, QXmlStreamWriter * writer);
  // virtual void readXML(QXmlStreamReader * reader) = 0;

  virtual ~Plugin();

  /// Creates a plugin from the name.
  static Plugin * createNamedPlugin(const QString & name);

  /// Register the given plugin creation function.
  static void registerPlugin(const QString & name, 
                             PluginFactory::Creator creator);
};

/// A helper class to ensure a plugin is registered from the start.
///
/// \warning This class creates a Plugin instance to get its name, so
/// avoid using it if the plugin creation has weird side-effects.
///
/// NO!!!
///
/// \todo This class should be the main object registered by the
/// plugin factory. In addition to what exists now, it should provide
/// a public name, a short description, and possibly over time author
/// information or things in this spirit.
class PluginDef {
public:
  PluginDef(PluginFactory::Creator c) {
    Plugin * p = c("");
    Plugin::registerPlugin(p->typeName(), c);
    delete p;
  };
};


#endif
