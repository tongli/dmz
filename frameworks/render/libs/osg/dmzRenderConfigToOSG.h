#ifndef DMZ_RENDER_CONFIG_TO_OSG_DOT_H
#define DMZ_RENDER_CONFIG_TO_OSG_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzTypesBase.h>
#include <osg/Vec4>

namespace dmz {

   class Config;

   osg::Vec4 config_to_osg_vec4_color ( const Config &Source);
   osg::Vec4 config_to_osg_vec4_color ( const Config &Source, const osg::Vec4 &Value);
   osg::Vec4 config_to_osg_vec4_color ( const String &Name, const Config &Source);
   osg::Vec4 config_to_osg_vec4_color ( 
      const String &Name, 
      const Config &Source, 
      const osg::Vec4 &DefaultValue);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const String &Name,
      const Config &Source) {

   const osg::Vec4 Value;
   return config_to_osg_vec4_color (Name, Source, Value);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const Config &Source,
      const osg::Vec4 &Value) {

   return config_to_osg_vec4_color ("", Source, Value);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const Config &Source) {

   const osg::Vec4 Value;
   return config_to_osg_vec4_color ("", Source, Value);
}

osg::Vec4
dmz::config_to_osg_vec4_color (
      const String &Name,
      const Config &Source,
      const osg::Vec4 &DefaultValue) {

   Config cd;

   if (Name) {

      Source.lookup_config (Name, cd);
   }
   else { cd = Source; }

   const osg::Vec4 Result (
      config_to_float32 ("r", cd, DefaultValue.x ()),
      config_to_float32 ("g", cd, DefaultValue.y ()),
      config_to_float32 ("b", cd, DefaultValue.z ()),
      DefaultValue.w ());

   return Result;
}

#endif // DMZ_RENDER_CONFIG_TO_OSG_DOT_H

