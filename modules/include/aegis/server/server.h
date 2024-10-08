#ifndef AEGIS_SERVER_SERVER_H
#define AEGIS_SERVER_SERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QHostAddress>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace grpc {

class Server;
class Service;
class ServerCompletionQueue;

}  // namespace grpc

namespace aegis {

class Service;

/* ------------------------------- IsValidService --------------------------- */

template <typename TYPE>
concept IsValidService = std::is_base_of_v<aegis::Service, TYPE> &&
    std::is_base_of_v<grpc::Service, TYPE>;

/* ----------------------------------- Server ------------------------------- */

class LIB_AEGIS_API Server {
 public:
  explicit Server();
  ~Server();

  void listen(const QHostAddress& host, quint16 port);

  template <IsValidService SERVICE, typename... ARGS>
  void registerService(ARGS&&... args);

 private:
  void startLoop();

  std::list<std::unique_ptr<Service>> m_services;
  std::unique_ptr<grpc::Server> m_server;
  std::unique_ptr<grpc::ServerCompletionQueue> m_queue;
};

template <IsValidService SERVICE, typename... ARGS>
void Server::registerService(ARGS&&... args) {
  m_services.emplace_back(
      std::make_unique<SERVICE>(std::forward<ARGS>(args)...));
}

}  // namespace aegis

#endif  // AEGIS_SERVER_SERVER_H