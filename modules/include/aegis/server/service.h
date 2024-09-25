#ifndef AEGIS_SERVER_SERVICE_H
#define AEGIS_SERVER_SERVICE_H

/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace grpc {

class Service;
class ServerCompletionQueue;

}  // namespace grpc

namespace aegis {

/* ----------------------------------- Service ------------------------------ */

class LIB_AEGIS_API Service {
 public:
  explicit Service() = default;
  virtual ~Service() = default;

  virtual void start(grpc::ServerCompletionQueue* queue) = 0;

  [[nodiscard]] virtual grpc::Service* grpc() = 0;
};

/* ------------------------------- ServiceWrapper --------------------------- */

template <typename GRPCService>
class ServiceWrapper : public GRPCService, public Service {
 public:
  explicit ServiceWrapper() = default;
  ~ServiceWrapper() override = default;

  [[nodiscard]] grpc::Service* grpc() override { return this; }
};

}  // namespace aegis

#endif  // AEGIS_SERVER_SERVICE_H