#ifndef AEGIS_COMMAND_CHILDREN_H
#define AEGIS_COMMAND_CHILDREN_H

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

/* ---------------------------- ObjectChildrenMessage ----------------------- */

struct LIB_AEGIS_API ObjectChildrenMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QString object MEMBER object)
  Q_PROPERTY(QStringList children MEMBER children)

  [[nodiscard]] bool operator==(const ObjectChildrenMessage& other) const;
  [[nodiscard]] bool operator!=(const ObjectChildrenMessage& other) const;

  QString object;
  QStringList children;
};

/* --------------------------- ObjectsChildrenMessage ----------------------- */

struct LIB_AEGIS_API ObjectsChildrenMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QList<ObjectChildrenMessage> objects MEMBER objects)

  QList<ObjectChildrenMessage> objects;
};

/* ------------------------------ ChildrenCommand --------------------------- */

class LIB_AEGIS_API ChildrenCommand : public Command {
 public:
  explicit ChildrenCommand();
  ~ChildrenCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<ObjectsChildrenMessage> find(
      const ObjectQuery& query) const;
  [[nodiscard]] QList<ObjectQuery> getChildren(const QObject* object) const;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_CHILDREN_H