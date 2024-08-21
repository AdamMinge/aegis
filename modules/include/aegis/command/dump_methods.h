#ifndef AEGIS_COMMAND_DUMP_METHODS_H
#define AEGIS_COMMAND_DUMP_METHODS_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QList>
#include <QStringList>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ---------------------------- MethodsDumpMessage -------------------------- */

struct LIB_AEGIS_API MethodsDumpMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QString object MEMBER object)
  Q_PROPERTY(QStringList methods MEMBER methods)

  [[nodiscard]] bool operator==(const MethodsDumpMessage& other) const;
  [[nodiscard]] bool operator!=(const MethodsDumpMessage& other) const;

  QString object;
  QStringList methods;
};

/* --------------------------- MethodsDumpsMessage ------------------------- */

struct LIB_AEGIS_API MethodsDumpsMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QList<MethodsDumpMessage> objects MEMBER objects)

  QList<MethodsDumpMessage> objects;
};

/* ---------------------------- DumpMethodsCommand -------------------------- */

class LIB_AEGIS_API DumpMethodsCommand : public Command {
 public:
  explicit DumpMethodsCommand();
  ~DumpMethodsCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<MethodsDumpsMessage> dump(
      const ObjectQuery& query) const;
  [[nodiscard]] QStringList getMethods(const QObject* object) const;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_DUMP_METHODS_H
