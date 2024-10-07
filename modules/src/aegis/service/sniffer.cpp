/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/sniffer.h"

#include "aegis/module.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SnifferStartCall -------------------------- */

SnifferStartCall::SnifferStartCall(aegis_proto::Sniffer::AsyncService* service,
                                   grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Sniffer::AsyncService::RequestStart) {}

SnifferStartCall::~SnifferStartCall() = default;

std::unique_ptr<SnifferStartCallData> SnifferStartCall::clone() const {
  return std::make_unique<SnifferStartCall>(getService(), getQueue());
}

SnifferStartCall::ProcessResult SnifferStartCall::process(
    const Request& request) const {
  if (sniffer().isSniffing()) {
    return {
        grpc::Status(
            grpc::StatusCode::INVALID_ARGUMENT,
            "The start cannot be triggered, the sniffer is already working"),
        {}};
  }

  sniffer().start();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ------------------------------ SnifferStopCall --------------------------- */

SnifferStopCall::SnifferStopCall(aegis_proto::Sniffer::AsyncService* service,
                                 grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Sniffer::AsyncService::RequestStop) {}

SnifferStopCall::~SnifferStopCall() = default;

std::unique_ptr<SnifferStartCallData> SnifferStopCall::clone() const {
  return std::make_unique<SnifferStopCall>(getService(), getQueue());
}

SnifferStopCall::ProcessResult SnifferStopCall::process(
    const Request& request) const {
  if (!sniffer().isSniffing()) {
    return {grpc::Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                "The stop cannot be triggered, the sniffer is already stopped"),
            {}};
  }

  sniffer().stop();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ----------------------------- SnifferListenCall -------------------------- */

SnifferListenCall::SnifferListenCall(
    aegis_proto::Sniffer::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : StreamCallData(service, queue, CallTag{this},
                     &aegis_proto::Sniffer::AsyncService::RequestListen) {}

SnifferListenCall::~SnifferListenCall() = default;

std::unique_ptr<SnifferListenCallData> SnifferListenCall::clone() const {
  return std::make_unique<SnifferListenCall>(getService(), getQueue());
}

SnifferListenCall::ProcessResult SnifferListenCall::process(
    const Request& request) const {
  return grpc::Status::OK;
}

/* ------------------------------- SnifferService -------------------------- */

SnifferService::SnifferService() = default;

SnifferService::~SnifferService() = default;

void SnifferService::start(grpc::ServerCompletionQueue* queue) {
  auto start_call = new SnifferStartCall(this, queue);
  auto stop_call = new SnifferStopCall(this, queue);
  auto listen_call = new SnifferListenCall(this, queue);

  start_call->proceed();
  stop_call->proceed();
  listen_call->proceed();
}

}  // namespace aegis
