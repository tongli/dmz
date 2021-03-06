#ifndef DMZ_QT_SPLASH_SCREEN_DOT_H
#define DMZ_QT_SPLASH_SCREEN_DOT_H

#include <dmzRuntimeLog.h>
#include <QtGui/QSplashScreen>


namespace dmz {

   class QtSplashScreen : public QSplashScreen, protected LogObserver {

      public:
         QtSplashScreen (RuntimeContext *context);
         ~QtSplashScreen ();

      protected:
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

      private:
         QtSplashScreen ();
         QtSplashScreen (const QtSplashScreen &);
         QtSplashScreen &operator= (const QtSplashScreen &);
   };
};

#endif // DMZ_QT_SPLASH_SCREEN_DOT_H

