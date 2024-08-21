#ifndef AEGIS_COMMAND_INVOKE_METHOD_H
#define AEGIS_COMMAND_INVOKE_METHOD_H

/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ----------------------------- InvokeMethodCommand ------------------------ */

class LIB_AEGIS_API InvokeMethodCommand : public Command {
 public:
  explicit InvokeMethodCommand();
  ~InvokeMethodCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<> invokeMethod(const ObjectQuery& query,
                                        const QString& name,
                                        const QStringList& values);

  [[nodiscard]] QMetaMethod getMethod(const QObject* object,
                                      const QString& name) const;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_INVOKE_METHOD_H
