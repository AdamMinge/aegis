#ifndef AEGIS_SERVICE_SNIFFER_H
#define AEGIS_SERVICE_SNIFFER_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/server/call.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SnifferStartCall -------------------------- */

using SnifferStartCallData =
    CallData<aegis_proto::Sniffer::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API SnifferStartCall : public SnifferStartCallData {
 public:
  explicit SnifferStartCall(aegis_proto::Sniffer::AsyncService* service,
                            grpc::ServerCompletionQueue* queue);
  ~SnifferStartCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<SnifferStartCallData> clone() const override;
};

/* ------------------------------ SnifferStartCall -------------------------- */

using SnifferStartCallData =
    CallData<aegis_proto::Sniffer::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API SnifferStopCall : public SnifferStartCallData {
 public:
  explicit SnifferStopCall(aegis_proto::Sniffer::AsyncService* service,
                           grpc::ServerCompletionQueue* queue);
  ~SnifferStopCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<SnifferStartCallData> clone() const override;
};

/* ------------------------------- SnifferService --------------------------- */

class LIB_AEGIS_API SnifferService : public aegis_proto::Sniffer::AsyncService {
 public:
  explicit SnifferService(grpc::ServerCompletionQueue* queue);
  ~SnifferService() override;
};

}  // namespace aegis

#endif  // AEGIS_SERVICE_SNIFFER_H
