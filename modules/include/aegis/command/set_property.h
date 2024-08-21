#ifndef AEGIS_COMMAND_SET_PROPERTY_H
#define AEGIS_COMMAND_SET_PROPERTY_H

/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ----------------------------- SetPropertyCommand ------------------------- */

class LIB_AEGIS_API SetPropertyCommand : public Command {
 public:
  explicit SetPropertyCommand();
  ~SetPropertyCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<> setProperty(const ObjectQuery& query,
                                       const QString& name,
                                       const QString& value);
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_SET_PROPERTY_H
