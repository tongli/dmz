#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystemFile.h>
#include <dmzXMLParser.h>
#include <dmzXMLInterpreterConfig.h>
#include <dmzXMLUtil.h>


#include <stdio.h>

int
main (int argc, char *argv[]) {

   dmz::Runtime rt;
   dmz::LogObserverBasic obs (rt.get_context ());

   if (argc == 2) {

      FILE *fd = dmz::open_file (argv[1], "rb");

      if (fd) {

         dmz::Config gdata ("global");
         dmz::XMLParser parser;
         dmz::XMLInterpreterConfig inter (gdata);
         parser.set_interpreter (&inter);

         char buffer[1056];
         dmz::Int32 size (0);

         do {

            size = fread (buffer, sizeof (char), 1056, fd);

            if (size && !parser.parse_buffer (buffer, size, size < 1056)) {

               size = 0;

               fprintf (stderr, "Error: %s\n", parser.get_error ().get_buffer ());
            }

         } while (size);

         dmz::Log log ("", rt.get_context ());
         dmz::write_xml_header (log.out);
         dmz::format_config_to_xml (gdata, log.out, dmz::True);

         dmz::close_file (fd);
      }
   }

   return 0;
}
