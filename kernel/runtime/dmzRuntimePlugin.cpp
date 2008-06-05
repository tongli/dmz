#include <dmzRuntimePlugin.h>

// Note: This file is not compiled in, it is strictly a holder for documentation.

/*!

\file dmzRuntimePlugin.h
\brief Defines the dmz::Plugin interface.

\class dmz::Plugin
\ingroup Runtime
\brief Plugin interface.
\details The Plugin class provide a pure virtual interface for creating pluigins,
modules, and extensions. Classes derived from Plugin must provide a factory function
in order to be loaded at runtime. This factory function must take the form of the
create_plugin_factory_function()
defined in dmzRuntimePlugin.h. The current
factory function naming convention is: create_<Class Name>. The factory function
should be wrapped in an extern "C" so that the C++ compiler does not mangle the
factory name. For example, a Plugin derived class name ObjectDump would have the
following factory function:
\code
#include <dmzRuntimePluginFactoryLinkSymbol.h>

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_ObjectDump (const dmz::PluginInfo &Info, dmz::Config &local, dmz::Config global) {

   return new ObjectDump (Info, local);
}

}
\endcode

\fn dmz::Plugin::Plugin (const PluginInfo &Info)
\brief Constructor.
\details The Plugin constructor initialized the plugin state that is common for
all Plugins. The PluginInfo object reference is typically passed in through the
factory function. The PluginInfo object should not be locally created as it contains
important runtime specific information.
\param[in] Info PluginInfo object reference.

\fn dmz::Plugin::~Plugin ()
\brief Destructor.

\fn dmz::Plugin::get_plugin_name () const
\brief Gets Plugin name.
\details The Plugin name is defined by the PluginInfo that was passed into the
constructor. Often the name returned
is the same as the name of the derived class. However when more than one Plugin of
the same type is loaded, it is possible to assign a unique name to each instance
so that they may be differentiated.
\return Returns a String containing the name of the Plugin.

\fn dmz::Plugin::get_plugin_handle () const
\brief Gets Plugin unique handle.
\details The Plugin's handle is a 32-bit number that is unique with in the runtime
context that was included in the PluginInfo object passed into the constructor.
\return Returns the Plugin's unique handle.

\fn dmz::Plugin::get_plugin_runtime_context () const
\brief Gets pointer to runtime context.
\details The PluginInfo object that was used to initialize the Plugin is associated
with a runtime context. This runtime context is often needed to create Runtime based
objects such as various runtime types. This function returns the runtime context used
to initialized the Plugin.
\return Returns pointer to runtime context.

\fn dmz::Plugin::discover_plugin (const Plugin *PluginPtr)
\brief Discover other plugins.
\details This function is used to pass the pointer of every Plugin stored in the
same PluginContainer as the current Plugin. Each Plugin pointer may be queried
to discover what module interface it supports. This function may be invoked multiple
times. One call is made for each Plugin being discovered.
\param[in] PluginPtr Pointer a Plugin.

\fn dmz::Plugin::start_plugin ()
\brief Start the Plugin.
\details This function is invoked after all the Plugins have been discovered.
It indicates that the runtime is about to being the main simulation loop.
Any set up that needs to be done before stating the sync phase should be done here.

\fn dmz::Plugin::stop_plugin ()
\brief Stop the Plugin.
\details This function is invoked to inform the Plugin that the sync loop is being
stopped. After a Plugin has been stopped, new Plugins may be discovered and
discovered Plugins may be removed.

\fn dmz::Plugin::shutdown_plugin ()
\brief Shutdown the Plugin.
\details This function is invoked to inform the Plugin that it is about to be deleted
and that all discovered Plugins are about to be removed.

\fn dmz::Plugin::remove_plugin (const Plugin *PluginPtr)
\brief Remove previously discovered Plugins.
\details This function may be invoked multiple times. One call is made for each
Plugin being removed.
\param[in] PluginPtr Pointer to the Plugin being removed.

\typedef dmz::Plugin *(*create_plugin_factory_function) (
   const dmz::PluginInfo &Info,
   dmz::Config &local,
   dmz::Config &global);
\brief Function pointer type used to create Plugins.

*/