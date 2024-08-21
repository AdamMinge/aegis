#ifndef AEGIS_COMMAND_EXECUTOR_H
#define AEGIS_COMMAND_EXECUTOR_H

/* --------------------------------- Standard ------------------------------- */
#include <unordered_map>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class Command;

/* ------------------------------- CommandExecutor -------------------------- */

class LIB_AEGIS_API CommandExecutor : public QObject {
  Q_OBJECT

 public:
  explicit CommandExecutor(QObject* parent = nullptr);
  ~CommandExecutor() override;

  QByteArray exec(const QByteArray& data);

  bool addCommand(std::unique_ptr<Command>&& command);
  std::unique_ptr<Command> takeCommand(const QString& name);
  bool removeCommand(const QString& name);

  QList<Command*> getCommands() const;

 private:
  std::unordered_map<QString, std::unique_ptr<Command>> m_commands;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_EXECUTOR_H