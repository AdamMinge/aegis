#ifndef AEGIS_SERVER_SERVER_H
#define AEGIS_SERVER_SERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QHostAddress>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/server/call.h"
/* -------------------------------------------------------------------------- */

namespace grpc {

class Service;
class Server;
class ServerCompletionQueue;

}  // namespace grpc

namespace aegis {

class Server {
 public:
  explicit Server();
  ~Server();

  void listen(const QHostAddress& host, quint16 port);

  template <typename GRPCService, typename... Args>
  void registerService(Args&&... args);

 private:
  void startLoop();

  std::list<std::unique_ptr<grpc::Service>> m_services;
  std::unique_ptr<grpc::Server> m_server;
  std::unique_ptr<grpc::ServerCompletionQueue> m_queue;
};

template <typename GRPCService, typename... Args>
void Server::registerService(Args&&... args) {
  m_services.emplace_back(std::make_unique<GRPCService>(
      m_queue.get(), std::forward<Args>(args)...));
}

}  // namespace aegis

#endif  // AEGIS_SERVER_SERVER_H