#ifndef AEGIS_COMMAND_DUMP_PROPERTIES_H
#define AEGIS_COMMAND_DUMP_PROPERTIES_H

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

/* --------------------------- PropertiesDumpMessage ------------------------ */

struct LIB_AEGIS_API PropertiesDumpMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QString object MEMBER object)
  Q_PROPERTY(QMap<QString, QVariant> properties MEMBER properties)

  [[nodiscard]] bool operator==(const PropertiesDumpMessage& other) const;
  [[nodiscard]] bool operator!=(const PropertiesDumpMessage& other) const;

  QString object;
  QMap<QString, QVariant> properties;
};

/* -------------------------- PropertiesDumpsMessage ----------------------- */

struct LIB_AEGIS_API PropertiesDumpsMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QList<PropertiesDumpMessage> objects MEMBER objects)

  QList<PropertiesDumpMessage> objects;
};

/* --------------------------- DumpPropertiesCommand ------------------------ */

class LIB_AEGIS_API DumpPropertiesCommand : public Command {
 public:
  explicit DumpPropertiesCommand();
  ~DumpPropertiesCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<PropertiesDumpsMessage> dump(
      const ObjectQuery& query) const;
  [[nodiscard]] QMap<QString, QVariant> getProperties(
      const QObject* object) const;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_DUMP_PROPERTIES_H
