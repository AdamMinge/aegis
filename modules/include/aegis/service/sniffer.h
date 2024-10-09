#ifndef AEGIS_SERVICE_SNIFFER_H
#define AEGIS_SERVICE_SNIFFER_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/server/call.h"
#include "aegis/server/service.h"
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

/* ------------------------------ SnifferStopCall --------------------------- */

using SnifferStopCallData =
    CallData<aegis_proto::Sniffer::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API SnifferStopCall : public SnifferStopCallData {
 public:
  explicit SnifferStopCall(aegis_proto::Sniffer::AsyncService* service,
                           grpc::ServerCompletionQueue* queue);
  ~SnifferStopCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<SnifferStopCallData> clone() const override;
};

/* --------------------------- SnifferListenCallData ------------------------ */

using SnifferListenCallData =
    StreamCallData<aegis_proto::Sniffer::AsyncService, google::protobuf::Empty,
                   aegis_proto::SnifferListenResponse>;

class LIB_AEGIS_API SnifferListenCall : public SnifferListenCallData {
 public:
  explicit SnifferListenCall(aegis_proto::Sniffer::AsyncService* service,
                             grpc::ServerCompletionQueue* queue);
  ~SnifferListenCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<SnifferListenCallData> clone() const override;
};

/* ------------------------------- SnifferService -------------------------- */

class SnifferService
    : public ServiceWrapper<aegis_proto::Sniffer::AsyncService> {
 public:
  explicit SnifferService();
  ~SnifferService() override;

  void start(grpc::ServerCompletionQueue* queue) override;
};

}  // namespace aegis

#endif  // AEGIS_SERVICE_SNIFFER_H
