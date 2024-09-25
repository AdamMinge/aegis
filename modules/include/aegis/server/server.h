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
class ServerCompletionQueue;

}  // namespace grpc

namespace aegis {

class Service;

/* ----------------------------------- Server ------------------------------- */

class LIB_AEGIS_API Server {
 public:
  explicit Server();
  ~Server();

  void listen(const QHostAddress& host, quint16 port);

  template <typename GRPCService, typename... Args>
  void registerService(Args&&... args);

 private:
  void startLoop();

  std::list<std::unique_ptr<Service>> m_services;
  std::unique_ptr<grpc::Server> m_server;
  std::unique_ptr<grpc::ServerCompletionQueue> m_queue;
};

template <typename Service, typename... Args>
void Server::registerService(Args&&... args) {
  m_services.emplace_back(
      std::make_unique<Service>(std::forward<Args>(args)...));
}

}  // namespace aegis

#endif  // AEGIS_SERVER_SERVER_H