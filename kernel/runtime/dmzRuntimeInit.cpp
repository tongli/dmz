#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextDefinitions.h"
#include "dmzRuntimeContextMessaging.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeHandleAllocator.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include "dmzRuntimeMessageContext.h"
#include "dmzRuntimePluginInfo.h"
#include "dmzRuntimeTypeContext.h"
#include <dmzRuntimeTime.h>
#include <dmzTypesMask.h>
#include <dmzTypesString.h>
#include <dmzTypesStringTokenizer.h>
#include <dmzTypesStringUtil.h>

/*!

\file dmzRuntimeInit.h
\brief File contains initialization function.
\details File defines a function for initializing the runtime types.

*/

using namespace dmz;

static const dmz::String LocalRootObjectTypeName ("Root_Object");
static const dmz::String LocalRootEventTypeName ("Root_Event");

namespace {

static void local_init_event_type (
   const Config &Init,
   RuntimeContextDefinitions &def,
   RuntimeContext *context,
   Log *log);

static void local_init_object_type (
   const Config &Init,
   RuntimeContextDefinitions &def,
   RuntimeContext *context,
   Log *log);

static void local_init_state (
   const Config &Init,
   RuntimeContextDefinitions &def,
   Log *log);

static void local_create_message_type (
   const String &Name,
   const Message *Parent,
   RuntimeContextMessaging &def,
   RuntimeContext *context,
   Log *log,
   Message &msg);

static void local_init_message_type (
   const Config &Init,
   RuntimeContextMessaging &def,
   RuntimeContext *context,
   Log *log);

static void local_init_time (
   const Config &Init,
   RuntimeContext *context,
   Log *log);


static void
local_init_event_type (
      const Config &Init,
      RuntimeContextDefinitions &def,
      RuntimeContext *context,
      Log *log) {

   Config current;
   ConfigIterator it;

   Definitions defs (context, log);

   EventType RootType (defs.get_root_event_type ());

   for (
         Boolean found = Init.get_first_config (it, current);
         found;
         found = Init.get_next_config (it, current)) {

      String name;

      if (current.lookup_attribute ("name", name)) {

         EventType *parent (0);

         String parentName;
         if (current.lookup_attribute ("parent", parentName)) {

            parent = def.eventNameTable.lookup (parentName);
         }
         else { parent = &RootType; }

         if (parent) {

            EventType *eventType = def.eventNameTable.lookup (name);

            if (eventType) {

               if (eventType->get_parent () == *parent) {

                  TypeContext *eventContext (eventType->get_type_context ());

                  if (eventContext) {

                     eventContext->lock.lock ();
                     if (eventContext->config) {

                        Config config (eventContext->config);
                        config.add_children (current);
                     }
                     else {

                        eventContext->config = current.get_config_context ();
                        if (eventContext->config) { eventContext->config->ref (); }
                     }
                     eventContext->lock.unlock ();
                  }
               }
               else if (log) {

                  const String ParentName (
                     eventType->get_parent ().get_name () ?
                        eventType->get_parent ().get_name () : "<Unknown>");

                  log->error << "Unable to re-define event type: "
                     << name << " because new parent: " << parentName
                     << " does not match original parent: "
                     << ParentName << endl;
               }
            }
            else {

               TypeContext *eventContext (new TypeContext (
                  name,
                  context,
                  parent->get_type_context (),
                  current.get_config_context ()));

               if (eventContext) {

                  EventType *eventType (new EventType (eventContext));

                  if (eventType) {

                     if (def.eventNameTable.store (name, eventType)) {

                        def.eventHandleTable.store (eventType->get_handle (), eventType);
                     }
                     else {

                        TypeContext *parentContext (
                           parent->get_type_context ());

                        if (parentContext) {

                           parentContext->lock.lock ();
                           if (parentContext->table.remove (eventType->get_handle ())) {

                              eventContext->unref ();
                           }
                           parentContext->lock.unlock ();
                        }

                        delete eventType; eventType = 0;

                        if (log) {

                           log->error << "Failed to create event type: "
                              << name
                              << ". Was possibly added by another thread "
                              << " concurrently?" << endl;
                        }
                     }
                  }

                  eventContext->unref ();
               }
            }
         }
         else if (log) {

             log->error << "event type: " << name << " has un-defined parent: "
                << (parentName ? parentName : "<Unknown>") << endl;
         }
      }
      else if (log) {

         log->warn << "Event type found with out a name while defining event types"
            << endl;
      }
   }
}


void
local_init_object_type (
      const Config &Init,
      RuntimeContextDefinitions &def,
      RuntimeContext *context,
      Log *log) {

   Config current;
   ConfigIterator it;

   Definitions defs (context, log);

   ObjectType RootType (defs.get_root_object_type ());

   for (
         Boolean found = Init.get_first_config (it, current);
         found;
         found = Init.get_next_config (it, current)) {

      String name;

      if (current.lookup_attribute ("name", name)) {

         ObjectType *parent (0);

         String parentName;
         if (current.lookup_attribute ("parent", parentName)) {

            parent = def.objectNameTable.lookup (parentName);
         }
         else { parent = &RootType; }

         if (parent) {

            ObjectType *objType = def.objectNameTable.lookup (name);

            if (objType) {

               if (objType->get_parent () == *parent) {

                  TypeContext *objContext (objType->get_type_context ());

                  if (objContext) {

                     objContext->lock.lock ();
                     if (objContext->config) {

                        Config config (objContext->config);
                        config.add_children (current);
                     }
                     else {

                        objContext->config = current.get_config_context ();
                        if (objContext->config) { objContext->config->ref (); }
                     }
                     objContext->lock.unlock ();
                  }
               }
               else if (log) {

                  const String ParentName (
                     objType->get_parent ().get_name () ?
                        objType->get_parent ().get_name () : "<Unknown>");

                  log->error << "Unable to re-define object type: "
                     << name << " because new parent: " << parentName
                     << " does not match original parent: "
                     << ParentName << endl;
               }
            }
            else {

               TypeContext *objContext (new TypeContext (
                  name,
                  context,
                  parent->get_type_context (),
                  current.get_config_context ()));

               if (objContext) {

                  ObjectType *objType (new ObjectType (objContext));

                  if (objType) {

                     if (def.objectNameTable.store (name, objType)) {

                        def.objectHandleTable.store (objType->get_handle (), objType);
                     }
                     else {

                        TypeContext *parentContext (
                           parent->get_type_context ());

                        if (parentContext) {

                           parentContext->lock.lock ();
                           if (parentContext->table.remove (objType->get_handle ())) {

                              objContext->unref ();
                           }
                           parentContext->lock.unlock ();
                        }

                        delete objType; objType = 0;

                        if (log) {

                           log->error << "Failed to create object type: "
                              << name
                              << ". Was possibly added by another thread "
                              << " concurrently?" << endl;
                        }
                     }
                  }

                  objContext->unref ();
               }
            }
         }
         else if (log) {

             log->error << "object type: " << name << " has un-defined parent: "
                << (parentName ? parentName : "<Unknown>") << endl;
         }
      }
      else if (log) {

         log->warn << "Object type found with out a name while defining object types"
            << endl;
      }
   }
}


void
local_init_state (
      const Config &Init,
      RuntimeContextDefinitions &def,
      Log *log) {

   ConfigIterator it;
   Config maskInit;

   for (
         Boolean found = Init.get_first_config (it, maskInit);
         found;
         found = Init.get_next_config (it, maskInit)) {

      const String Name = config_to_string ("name", maskInit);

      if (!def.maskTable.lookup (Name)) {

         def.maskShiftLock.lock ();
         const Int32 Shift (def.maskShift);
         def.maskShift++;
         def.maskShiftLock.unlock ();

         Mask *mask = new Mask;

         if (mask) {

            mask->set_sub_mask (0, 0x1);
            (*mask) << Shift;

            if (!def.maskTable.store (Name, mask)) {

               delete mask; mask = 0;

               if (log) {

                  log->error << "Unable to add new runtime state: " << Name << endl;
               }
            }
         }
      }
      else if (log) {

         log->error << "Runtime state: " << Name << " already defined." << endl;
      }
   }
}


void
local_create_message_type (
      const String &Name,
      const Message *Parent,
      RuntimeContextMessaging &def,
      RuntimeContext *context,
      Log *log,
      Message &msg) {

   MessageContext *ptr (new MessageContext (
      Name,
      context,
      &def,
      Parent ? Parent->get_message_type_context () : 0));

   if (ptr) {

      Message *type (new Message (ptr));

      if (type) {

         if (log) { log->info << "Created message type: " << Name << endl; }

         if (def.messageNameTable.store (type->get_name (), type)) {

            msg = *type;
            def.messageHandleTable.store (type->get_handle (), type);
         }
         else { delete type; type = 0; }
      }

      ptr->unref (); ptr = 0;
   }
}


void
local_init_message_type (
      const Config &Init,
      RuntimeContextMessaging &def,
      RuntimeContext *context,
      Log *log) {

   ConfigIterator it;
   Config current;

   for (
         Boolean found = Init.get_first_config (it, current);
         found;
         found = Init.get_next_config (it, current)) {

      String name;

      if (current.lookup_attribute ("name", name)) {

         Message *parent (0);

         String parentName;

         if (current.lookup_attribute ("parent", parentName)) {

            parent = def.messageNameTable.lookup (parentName);

            if (!parent && log) {

            }
         }

         Message *msg (def.messageNameTable.lookup (name));

         if (msg) {

            Message tmp;
            msg->get_parent (tmp);

            if (tmp.get_message_type_context () !=
                  (parent ? parent->get_message_type_context () : 0)) {

               if (log) {

                  log->error << "Message type: " << msg->get_name ()
                     << " with parent: " << (tmp.get_name () ? tmp.get_name () : "<NULL>")
                     << " attempted redefinition with parent: "
                     << (parent ? parent->get_name () : "<NULL>") << endl;
               }
            }
         }
         else {

            Message tmp;
            local_create_message_type (name, parent, def, context, log, tmp);
         }
      }
   }
}


void
local_init_time (const Config &Init, RuntimeContext *context, Log *log) {

   Time rtt (context);

   String data;

   Float64 factor (rtt.get_time_factor ());
   Boolean usingDefaultFactor (True);
   Float64 frequency (rtt.get_target_frame_frequency ());
   Boolean usingDefaultRate (True);

   if (Init.lookup_attribute ("factor.value", data)) {

      factor = string_to_float64 (data); 
      usingDefaultFactor = False;
      rtt.set_time_factor (factor);
   }

   if (Init.lookup_attribute ("frequency.value", data)) {

      frequency = string_to_float64 (data);
      usingDefaultRate = False;
      rtt.set_target_frame_frequency (frequency);
   }

   if (log) {

      log->debug << "Using " << (usingDefaultFactor ? "default " : "") << "time factor: "
         << factor << endl;

      log->debug << "Using " << (usingDefaultRate ? "default " : "")
         << "target frame frequency: " << frequency << endl;
   }
}

};

/*!

\ingroup Runtime
\brief Initialized the runtime types, state, and time.
\details Defined in dmzRuntimeInit.h. \n
An XML example for defining the various runtime types and time settings:
\code
<dmz>
<runtime>

   <time>
      <factor value="1.0"/>
      <frequency value="60"/>
   </time>

   <states>
      <!-- State definition -->
      <state name="State Name"/>
   </states>

   <types>
      <!-- dmz::ObjectType definition -->
      <object name="Object Name" parent="Parent Name">
         <!-- Object data -->
      </object>

      <!-- dmz::EventType definition -->
      <event name="Event Name" parent="Parent Name">
         <!-- Event data -->
      </event>

      <!-- dmz::Message definition -->
      <message name="Message Name" parent="Parent Name"/>
   </types>

</runtime>
</dmz>
\endcode
\param[in] Init Config containing runtime initialization data.
\param[in] context Pointer to runtime context.
\param[in] log Pointer to Log to use for log messages.
\sa dmz::ObjectType \n dmz::EventType \n dmz::Message \n dmz::Time

*/
void
dmz::runtime_init (const Config &Init, RuntimeContext *context, Log *log) {

   Config econfig;
   Config oconfig;
   Config sconfig;
   Config mconfig;
   Config tconfig;

   Init.lookup_all_config ("types.event", econfig);
   Init.lookup_all_config ("types.object", oconfig);
   Init.lookup_all_config ("states.state", sconfig);
   Init.lookup_all_config ("types.message", mconfig);
   Init.lookup_all_config_merged ("time", tconfig);

   if (context) {

      if (econfig || oconfig || sconfig) {

         RuntimeContextDefinitions *defs = context->get_definitions_context ();

         if (defs) {

            defs->ref ();
            if (econfig) { local_init_event_type (econfig, *defs, context, log); }
            else if (log) { log->debug << "Event type config not found" << endl; }
            if (oconfig) { local_init_object_type (oconfig, *defs, context, log); }
            else if (log) { log->debug << "Object type config not found" << endl; }
            if (sconfig) { local_init_state (sconfig, *defs, log); }
            else if (log) { log->debug << "State config not found" << endl; }
            defs->unref (); defs = 0;
         }
      }

      if (mconfig) {

         RuntimeContextMessaging *cm (context->get_messaging_context ());

         if (cm) {

            cm->ref ();
            local_init_message_type (mconfig, *cm, context, log);
            cm->unref ();
         }
      }
      else if (log) { log->debug << "Message type config not found" << endl; }

      if (tconfig) { local_init_time (tconfig, context, log); }
      else if (log) { log->debug << "Runtime time data not found" << endl; }
   }
}


/*!

\class dmz::Definitions
\ingroup Runtime
\brief Class for finding runtime types.
\details The Definitions class provides function for looking up the following types:
- dmz::Message
- dmz::EventType
- dmz::ObjectType
- States stored in dmz::Mask.

*/

struct dmz::Definitions::State {

   RuntimeContext *context;
   RuntimeContextDefinitions *defs;
   Log *log;

   State (RuntimeContext *theContext, Log *theLog) :
         context (theContext),
         defs (0),
         log (theLog) {

      if (context) {

         context->ref ();
         defs = context->get_definitions_context ();
         if (defs) { defs->ref (); }
      }
   }

   ~State () {

      if (defs) { defs->unref (); defs = 0; }
      if (context) { context->unref (); context = 0; }
      log = 0;
   }
};


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the Log to use for log messages. May be NULL.

*/
dmz::Definitions::Definitions (RuntimeContext *context, Log *log) :
      _state (*(new State (context, log))) {;}


/*!

\brief Constructor.
\param[in] Info PluginInfo used to obtain the runtime context.
\param[in] log Pointer to the Log to use for log messages. May be NULL.

*/
dmz::Definitions::Definitions (const PluginInfo &Info, Log *log) :
      _state (*(new State (Info.get_context (), log))) {;}


//! Destructor.
dmz::Definitions::~Definitions () { delete &_state; }


/*!

\brief Creates a named handle.
\param[in] Name String containing name of Handle.
\return Returns the named handle. Returns zero if the named handle could not be
created.

*/
dmz::Handle
dmz::Definitions::create_named_handle (const String &Name) {

   Handle result (0);

   if (Name && _state.context && _state.defs) {

      RuntimeHandle *handle (_state.defs->namedHandleTable.lookup (Name));

      if (!handle) {

         handle = new RuntimeHandle (Name + ".NamedHandle", _state.context);

         if (handle) {

            if (_state.defs->namedHandleTable.store (Name, handle)) {

               String *ptr (new String (Name));

               if (ptr) {

                  if (!_state.defs->namedHandleNameTable.store (
                        handle->get_runtime_handle (),
                        ptr)) {

                     delete ptr; ptr = 0;
                  }
               }
            }
            else {

               delete handle; handle = 0;
               handle = _state.defs->namedHandleTable.lookup (Name);
            }
         }
      }

      if (handle) { result = handle->get_runtime_handle (); }
   }

   return result;
}


/*!

\brief Looks up named handle.
\param[in] Name String containing name of Handle.
\return Returns the named handle if it exists. Returns zero if the named handle
does not exists.

*/
dmz::Handle
dmz::Definitions::lookup_named_handle (const String &Name) const {

   Handle result (0);

   if (_state.defs) {

      RuntimeHandle *handle (_state.defs->namedHandleTable.lookup (Name));

      if (handle) { result = handle->get_runtime_handle (); }
   }

   return result;
}


/*!

\brief Looks up named handle name.
\param[in] NamedHandle Handle used to find name.
\return Returns a String containing the name of the named handle if it exists.
Returns the empty string if the named handle is invalid.

*/
dmz::String
dmz::Definitions::lookup_named_handle_name (const Handle NamedHandle) const {

   String result;

   if (_state.defs) {

      String *ptr (_state.defs->namedHandleNameTable.lookup (NamedHandle));

      if (ptr) { result = *ptr; }
   }

   return result;
}


dmz::Message
dmz::Definitions::get_global_message_type () const {

   Message result;

   if (_state.context) {

      RuntimeContextMessaging *rcm = _state.context->get_messaging_context ();

      if (rcm) {

         rcm->ref ();

         result = rcm->globalType;

         rcm->unref ();
      }
      else if (_state.log) {

         _state.log->error << "Internal Error, Unable to get Global Message Type."
            << endl;
      }
   }
   else if (_state.log) {

      _state.log->error << "NULL Runtime context. Unable to get Global Message Type."
         << endl;
   }

   return result;
}


/*!

\brief Creates new message type context.
\param[in] Name String containing name of new message type context.
\param[out] type Message to store new message type context.
\return Returns dmz::True if a new message type context was created or a message type
context of the name name already exists.

*/
dmz::Boolean
dmz::Definitions::create_message_type (const String &Name, Message &type) {

   Boolean result (False);

   if (_state.context) {

      RuntimeContextMessaging *rcm = _state.context->get_messaging_context ();

      if (rcm) {

         rcm->ref ();

         Message *ptr (rcm->messageNameTable.lookup (Name));

         if (ptr) { type = *ptr; }
         else {

            local_create_message_type (Name, 0, *rcm, _state.context, _state.log, type);
         }

         if (type) { result = True; }
         else if (_state.log) {

            _state.log->warn << "Unable to create dmz::Message: " << Name << endl;
         }

         rcm->unref ();
      }
      else if (_state.log) {

         _state.log->error << "Internal Error, Unable to create dmz::MessgeType: "
            << Name << endl;
      }
   }
   else if (_state.log) {

      _state.log->error << "NULL Runtime context. Unable to create dmz::Message: "
         << Name << endl;
   }

   return result;
}


/*!

\brief Looks up message type context by name.
\param[in] Name String containing name of message type context.
\param[out] type Message to store found message type context.
\return Returns dmz::True if the named message type context is found.

*/
dmz::Boolean
dmz::Definitions::lookup_message_type (const String &Name, Message &type) const {

   Boolean result (False);

   if (_state.context) {

      RuntimeContextMessaging *rcm = _state.context->get_messaging_context ();

      if (rcm) {

         rcm->ref ();
         Message *ptr (rcm->messageNameTable.lookup (Name));
         if (ptr) { type = *ptr; result = True; }
         else if (_state.log) {

            _state.log->warn << "Unable to find dmz::Message: " << Name << endl;
         }

         rcm->unref ();
      }
      else if (_state.log) {

         _state.log->error << "Internal Error, Unable to lookup dmz::MessgeType: "
            << Name << endl;
      }
   }
   else if (_state.log) {

      _state.log->error << "NULL Runtime context. Unable to lookup dmz::Message: "
         << Name << endl;
   }

   return result;
}


/*!

\brief Looks up message type context by unique handle.
\param[in] TypeHandle Unique handle of message type context.
\param[out] type Message to store found message type context.
\return Returns dmz::True if the message type context is found.

*/
dmz::Boolean
dmz::Definitions::lookup_message_type (const Handle TypeHandle, Message &type) const {

   Boolean result (False);

   if (_state.context) {

      RuntimeContextMessaging *rcm = _state.context->get_messaging_context ();

      if (rcm) {

         rcm->ref ();
         Message *ptr (rcm->messageHandleTable.lookup (TypeHandle));
         if (ptr) { type = *ptr; result = True; }
         else if (_state.log) {

            _state.log->warn << "Unable to find dmz::Message: " << TypeHandle << endl;
         }

         rcm->unref ();
      }
      else if (_state.log) {

         _state.log->error << "Internal Error, Unable to lookup dmz::MessgeType: "
            << TypeHandle << endl;
      }
   }
   else if (_state.log) {

      _state.log->error << "NULL Runtime context. Unable to lookup dmz::Message: "
         << TypeHandle << endl;
   }

   return result;
}


//! Gets root event type.
dmz::EventType
dmz::Definitions::get_root_event_type () const {

   EventType result;

   if (_state.context && _state.defs) {

      EventType *ptr (_state.defs->eventNameTable.lookup (LocalRootEventTypeName));

      if (!ptr) {

         TypeContext *rootTypeContext = new TypeContext (
            LocalRootEventTypeName,
            _state.context,
            0,
            0);

         if (rootTypeContext) {

            ptr = new EventType (rootTypeContext);

            if (ptr && _state.defs->eventNameTable.store (ptr->get_name (), ptr)) {

               _state.defs->eventHandleTable.store (ptr->get_handle (), ptr);
            }
            else if (ptr) { delete ptr; ptr = 0; }

            rootTypeContext->unref ();
         }
      }

      if (ptr) { result = *ptr; }
   }

   return result;
}


/*!

\brief Looks up event type context by name.
\param[in] Name String containing name of event type context.
\param[out] type EventType used to return found event type context.
\return Returns dmz::True if the name event type context was found.

*/
dmz::Boolean
dmz::Definitions::lookup_event_type (const String &Name, EventType &type) const {

   Boolean result (False);

   if (_state.defs) {

      EventType *found (_state.defs->eventNameTable.lookup (Name));
      if (found) { type = *found; result = (type == *found); }
   }

   return result;
}


/*!

\brief Looks up event type context by unique handle.
\param[in] TypeHandle Unique handle of event type context.
\param[out] type EventType used to return found event type context.
\return Returns dmz::True if the name event type context was found.

*/
dmz::Boolean
dmz::Definitions::lookup_event_type (const Handle TypeHandle, EventType &type) const {

   Boolean result (False);

   if (_state.defs) {

      EventType *found (_state.defs->eventHandleTable.lookup (TypeHandle));
      if (found) { type = *found; result = (type == *found); }
   }

   return result;
}


//! Gets root object type.
dmz::ObjectType
dmz::Definitions::get_root_object_type () const {

   ObjectType result;

   if (_state.context && _state.defs) {

      ObjectType *ptr (_state.defs->objectNameTable.lookup (LocalRootObjectTypeName));

      if (!ptr) {

         TypeContext *rootTypeContext = new TypeContext (
            LocalRootObjectTypeName,
            _state.context,
            0,
            0);

         if (rootTypeContext) {

            ptr = new ObjectType (rootTypeContext);

            if (ptr && _state.defs->objectNameTable.store (ptr->get_name (), ptr)) {

               _state.defs->objectHandleTable.store (ptr->get_handle (), ptr);
            }
            else if (ptr) { delete ptr; ptr = 0; }

            rootTypeContext->unref ();
         }
      }

      if (ptr) { result = *ptr; }
   }

   return result;
}


/*!

\brief Looks up object type context by name.
\param[in] Name String containing name of object type context.
\param[out] type ObjectType used to return found object type context.
\return Returns dmz::True if the name object type context was found.

*/
dmz::Boolean
dmz::Definitions::lookup_object_type (const String &Name, ObjectType &type) const {

   Boolean result (False);

   if (_state.defs) {

      ObjectType *found (_state.defs->objectNameTable.lookup (Name));
      if (found) { type = *found; result = (type == *found); }
   }

   return result;
}


/*!

\brief Looks up object type context by unique handle.
\param[in] TypeHandle Unique handle of object type context.
\param[out] type ObjectType used to return found object type context.
\return Returns dmz::True if the name object type context was found.

*/
dmz::Boolean
dmz::Definitions::lookup_object_type (const Handle TypeHandle, ObjectType &type) const {

   Boolean result (False);

   if (_state.defs) {

      ObjectType *found (_state.defs->objectHandleTable.lookup (TypeHandle));
      if (found) { type = *found; result = (type == *found); }
   }

   return result;
}


/*!

\brief Looks up states from names.
\param[in] Name String containing state names.
\param[out] state Mask object to store found states.
\return Returns dmz::True if all named states are found. 
\note State names should be delineated by the "|" character (a.k.a. the bitwise or
operator).

*/
dmz::Boolean
dmz::Definitions::lookup_state (const String &Name, Mask &state) const {

   Boolean result (False);

   if (_state.defs) {

      StringTokenizer st (Name, '|');
      String value = st.get_next ();

      result = True;

      while (value) {

         trim_ascii_white_space (value);
         convert_ascii_white_space (value);

         Mask *ptr (_state.defs->maskTable.lookup (value));

         if (ptr) { state |= *ptr; }
         else if (_state.log) {

            _state.log->error << "Unable to find state: " << value
               << " in state name: " << Name << endl;

            result = False;
         }

         value = st.get_next ();
      }
   }

   return result;
}


/*!

\brief Gets state names.
\param[in] State Mask containing states to be named.
\param[out] name String to store found state names.
\return Returns dmz::True if all the states stored in \a State have names.
\note This function will find the names for as many states as possible. If the \a State
mask contains more than one state, the names are separated by the "|" character
(a.k.a. bitwise or operator).

*/
dmz::Boolean
dmz::Definitions::lookup_state_name (const Mask &State, String &name) const {

   Boolean result (False);

   Mask maskCopy (State);

   if (_state.defs) {

      HashTableStringIterator it;

      Mask *ptr = _state.defs->maskTable.get_first (it);

      while (ptr) {

         if (State & *ptr) {

            if (!name) { name = it.get_hash_key (); }
            else { name << " | " << it.get_hash_key (); }

            maskCopy.unset (*ptr);
         }

         ptr = _state.defs->maskTable.get_next (it);
      }

      if (!maskCopy) { result = True; }
      else if (_state.log) {

         _state.log->error << "Unidentified state bits in: " << name << endl;
            // << ", unknown bits: " << maskCopy << endl;
      }
   }

   return result;
}


/*!

\brief Creates a unique name.
\param[in] Name String containing unique name to create.
\return Returns dmz::True if the name is unique and has not already been created.
Returns dmz::False if the name has already been created.

*/
dmz::Boolean
dmz::Definitions::create_unique_name (const String &Name) {

   Boolean result (False);

   if (_state.defs) {

      _state.defs->uniqueNameLock.lock ();
         result = _state.defs->uniqueNameTable.store (Name, (void *)this);
      _state.defs->uniqueNameLock.unlock ();
   }

   return result;
}


/*!

\brief Looks up if unique name has been created.
\param[in] Name String containing unique name to check.
\return Returns dmz::True if the unique name exists. Returns dmz::False if the
unique name is unknown.

*/
dmz::Boolean
dmz::Definitions::lookup_unique_name (const String &Name) {

   Boolean result (False);

   if (_state.defs) {

      _state.defs->uniqueNameLock.lock ();
         result = (_state.defs->uniqueNameTable.lookup (Name) != 0);
      _state.defs->uniqueNameLock.unlock ();
   }

   return result;
}


/*!

\brief Release a unique name.
\param[in] Name String containing the unique name to release.
\return Returns dmz::True if the unique name was released. Returns dmz::False
if the unique name is unknown.

*/
dmz::Boolean
dmz::Definitions::release_unique_name (const String &Name) {

   Boolean result (False);

   if (_state.defs) {

      _state.defs->uniqueNameLock.lock ();
         result = (_state.defs->uniqueNameTable.remove (Name) != 0);
      _state.defs->uniqueNameLock.unlock ();
   }

   return result;
}


/*!

\brief Gets name associated with unique runtime handle.
\param[in] TypeHandle Unique runtime handle.
\return Returns String containing name associated with unique runtime handle.
Will return empty string if there is no name associated with handle.

*/
dmz::String
dmz::Definitions::lookup_runtime_name (const Handle TypeHandle) const {

   String result;

   if (_state.context) {

      HandleAllocator *info (_state.context->get_handle_allocator ());

      if (info) { result = info->lookup_info (TypeHandle); }
   }

   return result;
}
