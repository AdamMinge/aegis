/* ----------------------------------- Local -------------------------------- */
#include "aegis/search/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace aegis {

QObjectList getTopLevelObjects() {
  auto topLevelObjects = QObjectList{};
  for (const auto object : qApp->topLevelWidgets()) {
    if (!object->parent()) { topLevelObjects.append(object); }
  }
  return topLevelObjects;
}

}// namespace aegis
