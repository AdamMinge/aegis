#ifndef AEGIS_SERVICE_MARKER_H
#define AEGIS_SERVICE_MARKER_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/server/call.h"
#include "aegis/server/service.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ MarkerStartCall -------------------------- */

using MarkerStartCallData =
    CallData<aegis_proto::Marker::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API MarkerStartCall : public MarkerStartCallData {
 public:
  explicit MarkerStartCall(aegis_proto::Marker::AsyncService* service,
                           grpc::ServerCompletionQueue* queue);
  ~MarkerStartCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<MarkerStartCallData> clone() const override;
};

/* ------------------------------ MarkerStartCall -------------------------- */

using MarkerStartCallData =
    CallData<aegis_proto::Marker::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API MarkerStopCall : public MarkerStartCallData {
 public:
  explicit MarkerStopCall(aegis_proto::Marker::AsyncService* service,
                          grpc::ServerCompletionQueue* queue);
  ~MarkerStopCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<MarkerStartCallData> clone() const override;
};

/* ------------------------------- MarkerService --------------------------- */

class MarkerService : public ServiceWrapper<aegis_proto::Marker::AsyncService> {
 public:
  explicit MarkerService();
  ~MarkerService() override;

  void start(grpc::ServerCompletionQueue* queue) override;
};

}  // namespace aegis

#endif  // AEGIS_SERVICE_MARKER_H
