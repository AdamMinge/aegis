/* ----------------------------------- Local -------------------------------- */
#include "aegis/search/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace aegis {

QList<QObject *> getTopLevelObjects() {
  auto topLevelObjects = QList<QObject *>{};
  for (const auto object : qApp->topLevelWidgets()) {
    if (!object->parent()) { topLevelObjects.append(object); }
  }
  return topLevelObjects;
}

}// namespace aegis
