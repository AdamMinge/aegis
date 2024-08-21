#ifndef AEGIS_COMMAND_FIND_H
#define AEGIS_COMMAND_FIND_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QTimer>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ---------------------------- FoundObjectsMessage ------------------------- */

struct LIB_AEGIS_API FoundObjectsMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QStringList objects MEMBER objects)

  QStringList objects;
};

/* ------------------------------- FindCommand ------------------------------ */

class LIB_AEGIS_API FindCommand : public Command {
 public:
  explicit FindCommand();
  ~FindCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<FoundObjectsMessage> find(
      const ObjectQuery& query) const;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_FIND_H