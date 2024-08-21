/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/manager.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- CommandExecutor -------------------------- */

CommandExecutor::CommandExecutor(QObject* parent) : QObject(parent) {}

CommandExecutor::~CommandExecutor() = default;

QByteArray CommandExecutor::exec(const QByteArray& data) {
  const auto splited_data = data.split(' ');
  const auto command_name = splited_data.front();

  auto response = QByteArray{};
  if (auto iter = m_commands.find(command_name); iter != m_commands.end()) {
    auto args = QStringList{};
    std::transform(splited_data.constBegin(), splited_data.constEnd(),
                   std::back_inserter(args),
                   [](const auto& data) { return QString::fromUtf8(data); });

    response = (*iter).second->exec(args);
  } else {
    auto error = Response<>(ErrorMessage(
        QLatin1String("Process Request Error"),
        QLatin1String("Cannot find command: %1").arg(command_name)));

    response = serializer()->serialize(error);
  }

  return response;
}

bool CommandExecutor::addCommand(std::unique_ptr<Command>&& command) {
  if (m_commands.contains(command->getName())) return false;

  m_commands.insert(std::make_pair(command->getName(), std::move(command)));
  return true;
}

std::unique_ptr<Command> CommandExecutor::takeCommand(const QString& name) {
  if (!m_commands.contains(name)) return nullptr;

  auto command = std::move(m_commands[name]);
  m_commands.erase(name);

  return command;
}

bool CommandExecutor::removeCommand(const QString& name) {
  if (!m_commands.contains(name)) return false;

  m_commands.erase(name);
  return true;
}

QList<Command*> CommandExecutor::getCommands() const {
  auto commands = QList<Command*>{};
  for (auto& command : m_commands) {
    commands.append(command.second.get());
  }

  return commands;
}

}  // namespace aegis
