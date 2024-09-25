/* ----------------------------------- Local -------------------------------- */
#include "aegis/server/server.h"

#include "aegis/server/call.h"
#include "aegis/server/service.h"
/* ----------------------------------- GRPC --------------------------------- */
#include <grpc++/grpc++.h>
#include <grpc++/impl/service_type.h>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------------- Server ------------------------------- */

Server::Server() = default;

Server::~Server() {
  m_server->Shutdown();
  m_queue->Shutdown();
}

void Server::listen(const QHostAddress& host, quint16 port) {
  const auto address =
      QLatin1String("%1:%2").arg(host.toString()).arg(port).toStdString();

  grpc::ServerBuilder builder;
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  for (const auto& service : m_services)
    builder.RegisterService(service->grpc());

  m_queue = builder.AddCompletionQueue();
  m_server = builder.BuildAndStart();

  startLoop();
}

void Server::startLoop() {
  auto tag = (void*)nullptr;
  auto ok = false;

  for (const auto& service : m_services) service->start(m_queue.get());

  while (true) {
    if (m_queue->Next(&tag, &ok) && ok) {
      auto call_tag = static_cast<CallTag*>(tag);

      auto callable = static_cast<Callable*>(call_tag->callable);
      if (callable) {
        callable->proceed();
      }
    } else {
      // TODO !!!
    }
  }
}

}  // namespace aegis
