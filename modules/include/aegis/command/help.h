#ifndef AEGIS_COMMAND_HELP_H
#define AEGIS_COMMAND_HELP_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QTimer>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- CommandsHelpMessage ------------------------ */

struct LIB_AEGIS_API CommandsHelpMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QMap<QString, QString> helps MEMBER helps)

  QMap<QString, QString> helps;
};

/* ----------------------------- CommandsListMessage ------------------------ */

struct LIB_AEGIS_API CommandsListMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QStringList commands MEMBER commands)

  QStringList commands;
};

/* ------------------------------- HelpCommand ------------------------------ */

class LIB_AEGIS_API HelpCommand : public Command {
 public:
  explicit HelpCommand();
  ~HelpCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  [[nodiscard]] Response<CommandsHelpMessage> helps();
  [[nodiscard]] Response<CommandsListMessage> commands();
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_HELP_H