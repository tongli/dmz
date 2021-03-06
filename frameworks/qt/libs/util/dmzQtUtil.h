#ifndef DMZ_QT_UTIL_DOT_H
#define DMZ_QT_UTIL_DOT_H

#include <dmzQtUtilExport.h>
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>

class QObject;
class QWidget;


namespace dmz {

   class Config;

   QPointF to_qpointf (const Vector &Source);
   Vector to_dmz_vector (const QPointF &Source);

   QSizeF to_qsizef (const Vector &Source);
   Vector to_dmz_vector (const QSizeF &Source);

   DMZ_QT_UTIL_LINK_SYMBOL Float64 get_qmatrix_scale (const QMatrix &Source);

   void set_qobject_properties (const Config &Source, QObject *object);

   DMZ_QT_UTIL_LINK_SYMBOL void set_qobject_properties (
      const String &Name,
      const Config &Source,
      QObject *object);

   Boolean set_qwidget_stylesheet (const Config &Source, QWidget *widget);

   DMZ_QT_UTIL_LINK_SYMBOL Boolean set_qwidget_stylesheet (
      const String &Name,
      const Config &Source,
      QWidget *widget);
};


inline QPointF
dmz::to_qpointf (const Vector &Source) {

   QPointF result (Source.get_x (), Source.get_y ());
   return result;
}


inline dmz::Vector
dmz::to_dmz_vector (const QPointF &Source) {

   Vector result (Source.x (), Source.y (), 0.0);
   return result;
}


inline QSizeF
dmz::to_qsizef (const Vector &Source) {

   QSizeF result (Source.get_x (), Source.get_y ());
   return result;
}


inline dmz::Vector
dmz::to_dmz_vector (const QSizeF &Source) {

   Vector result (Source.width (), Source.height (), 0.0);
   return result;
}


// inline QMatrix
// dmz::to_qmatrix (const Matrix &Source) {
//
//    Float64 data[9];
//    Source.to_array (data);
//
//    QMatrix result (data[0], data[1], data[2], data[3], data[5], data[6]);
//    return result;
// }


inline void
dmz::set_qobject_properties (const Config &Source, QObject *object) {

   set_qobject_properties ("", Source, object);
}


inline dmz::Boolean
dmz::set_qwidget_stylesheet (const Config &Source, QWidget *widget) {

   return set_qwidget_stylesheet ("", Source, widget);
}


#endif // DMZ_QT_UTIL_DOT_H

