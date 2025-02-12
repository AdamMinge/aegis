/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/marker.h"

#include "aegis/module.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ MarkerStartCall -------------------------- */

MarkerStartCall::MarkerStartCall(
  aegis_proto::Marker::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Marker::AsyncService::RequestStart) {}

MarkerStartCall::~MarkerStartCall() = default;

std::unique_ptr<MarkerStartCallData> MarkerStartCall::clone() const {
  return std::make_unique<MarkerStartCall>(getService(), getQueue());
}

MarkerStartCall::ProcessResult
MarkerStartCall::process(const Request &request) const {
  if (marker().isMarking()) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        "The start cannot be triggered, the marker is already working"),
      {}};
  }

  marker().start();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ------------------------------ MarkerStopCall --------------------------- */

MarkerStopCall::MarkerStopCall(
  aegis_proto::Marker::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Marker::AsyncService::RequestStop) {}

MarkerStopCall::~MarkerStopCall() = default;

std::unique_ptr<MarkerStartCallData> MarkerStopCall::clone() const {
  return std::make_unique<MarkerStopCall>(getService(), getQueue());
}

MarkerStopCall::ProcessResult
MarkerStopCall::process(const Request &request) const {
  if (!marker().isMarking()) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        "The stop cannot be triggered, the marker is already stopped"),
      {}};
  }

  marker().stop();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ------------------------------- MarkerService --------------------------- */

MarkerService::MarkerService() = default;

MarkerService::~MarkerService() = default;

void MarkerService::start(grpc::ServerCompletionQueue *queue) {
  auto start_call = new MarkerStartCall(this, queue);
  auto stop_call = new MarkerStopCall(this, queue);

  start_call->proceed();
  stop_call->proceed();
}

}// namespace aegis
