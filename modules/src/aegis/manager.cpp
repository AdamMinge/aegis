/* ----------------------------------- Local -------------------------------- */
#include "aegis/manager.h"

#include "aegis/command/children.h"
#include "aegis/command/dump_methods.h"
#include "aegis/command/dump_properties.h"
#include "aegis/command/find.h"
#include "aegis/command/help.h"
#include "aegis/command/invoke_method.h"
#include "aegis/command/parent.h"
#include "aegis/command/record.h"
#include "aegis/command/set_property.h"
#include "aegis/command/sniffer.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

std::unique_ptr<AegisManager> AegisManager::m_instance =
    std::unique_ptr<AegisManager>(nullptr);

AegisManager& AegisManager::getInstance() {
  if (!m_instance) m_instance.reset(new AegisManager);

  return *m_instance;
}

void AegisManager::deleteInstance() { m_instance.reset(nullptr); }

AegisManager::AegisManager()
    : m_server(new Server(this)),
      m_searcher(new Searcher(this)),
      m_serializer(new Serializer(Serializer::Format::Json, this)),
      m_command_executor(new CommandExecutor(this)) {
  m_command_executor->addCommand(std::make_unique<ChildrenCommand>());
  m_command_executor->addCommand(std::make_unique<DumpMethodsCommand>());
  m_command_executor->addCommand(std::make_unique<DumpPropertiesCommand>());
  m_command_executor->addCommand(std::make_unique<FindCommand>());
  m_command_executor->addCommand(std::make_unique<HelpCommand>());
  m_command_executor->addCommand(std::make_unique<InvokeMethodCommand>());
  m_command_executor->addCommand(std::make_unique<ParentCommand>());
  m_command_executor->addCommand(std::make_unique<RecordCommand>());
  m_command_executor->addCommand(std::make_unique<SetPropertyCommand>());
  m_command_executor->addCommand(std::make_unique<SnifferCommand>());
}

AegisManager::~AegisManager() = default;

Server* AegisManager::getServer() const { return m_server; }

Searcher* AegisManager::getSearcher() const { return m_searcher; }

Serializer* AegisManager::getSerializer() const { return m_serializer; }

CommandExecutor* AegisManager::getCommandExecutor() const {
  return m_command_executor;
}

}  // namespace aegis
