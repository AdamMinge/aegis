#ifndef AEGIS_MANAGER_H
#define AEGIS_MANAGER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QTcpServer>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/executor.h"
#include "aegis/export.h"
#include "aegis/search/searcher.h"
#include "aegis/serializer.h"
#include "aegis/server.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- AegisManager ----------------------------- */

class LIB_AEGIS_API AegisManager : public QObject {
  Q_OBJECT

 public:
  [[nodiscard]] static AegisManager& getInstance();
  static void deleteInstance();

 public:
  ~AegisManager() override;

  [[nodiscard]] Server* getServer() const;
  [[nodiscard]] Searcher* getSearcher() const;
  [[nodiscard]] Serializer* getSerializer() const;
  [[nodiscard]] CommandExecutor* getCommandExecutor() const;

 protected:
  explicit AegisManager();

 private:
  static std::unique_ptr<AegisManager> m_instance;

  Server* m_server;
  Searcher* m_searcher;
  Serializer* m_serializer;
  CommandExecutor* m_command_executor;
};

inline Server* server() { return AegisManager::getInstance().getServer(); }

inline Searcher* searcher() {
  return AegisManager::getInstance().getSearcher();
}

inline Serializer* serializer() {
  return AegisManager::getInstance().getSerializer();
}

inline CommandExecutor* commandExecutor() {
  return AegisManager::getInstance().getCommandExecutor();
}

}  // namespace aegis

#endif  // AEGIS_MANAGER_H