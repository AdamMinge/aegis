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
};

/* ------------------------------- ServiceWrapper --------------------------- */

template <typename GRPC_SERVICE>
class ServiceWrapper : public GRPC_SERVICE, public Service {
 public:
  explicit ServiceWrapper() = default;
  ~ServiceWrapper() override = default;
};

}  // namespace aegis

#endif  // AEGIS_SERVER_SERVICE_H