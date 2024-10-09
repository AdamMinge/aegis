/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/recorder.h"

#include "aegis/module.h"
#include "aegis/record/action.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------- RecordedActionsMapper ------------------------ */

class RecordedActionsMapper {
 public:
  QString operator()(const RecordedAction::ContextMenuOpened& action) const {
    return QString{"ContextMenuOpened %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::ButtonClicked& action) const {
    return QString{"ButtonClicked %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::ButtonToggled& action) const {
    return QString{"ButtonToggled %1"}.arg(action.object.toString());
  }

  QString operator()(
      const RecordedAction::ComboBoxCurrentChanged& action) const {
    return QString{"ComboBoxCurrentChanged %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::SpinBoxValueChanged& action) const {
    return QString{"SpinBoxValueChanged %1"}.arg(action.object.toString());
  }

  QString operator()(
      const RecordedAction::DoubleSpinBoxValueChanged& action) const {
    return QString{"DoubleSpinBoxValueChanged %1"}.arg(
        action.object.toString());
  }

  QString operator()(const RecordedAction::SliderValueChanged& action) const {
    return QString{"SliderValueChanged %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::TabCurrentChanged& action) const {
    return QString{"TabCurrentChanged %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::TabClosed& action) const {
    return QString{"TabClosed %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::TabMoved& action) const {
    return QString{"TabMoved %1"}.arg(action.object.toString());
  }

  QString operator()(
      const RecordedAction::ToolBoxCurrentChanged& action) const {
    return QString{"ToolBoxCurrentChanged %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::ActionTriggered& action) const {
    return QString{"ActionTriggered %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::TextEditTextChanged& action) const {
    return QString{"TextEditTextChanged %1"}.arg(action.object.toString());
  }

  QString operator()(const RecordedAction::LineEditTextChanged& action) const {
    return QString{"LineEditTextChanged %1"}.arg(action.object.toString());
  }

  QString operator()(
      const RecordedAction::LineEditReturnPressed& action) const {
    return QString{"LineEditReturnPressed %1"}.arg(action.object.toString());
  }
};

/* ------------------------------ RecorderStartCall ------------------------- */

RecorderStartCall::RecorderStartCall(
    aegis_proto::Recorder::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Recorder::AsyncService::RequestStart) {}

RecorderStartCall::~RecorderStartCall() = default;

std::unique_ptr<RecorderStartCallData> RecorderStartCall::clone() const {
  return std::make_unique<RecorderStartCall>(getService(), getQueue());
}

RecorderStartCall::ProcessResult RecorderStartCall::process(
    const Request& request) const {
  if (recorder().isRecording()) {
    return {
        grpc::Status(
            grpc::StatusCode::INVALID_ARGUMENT,
            "The start cannot be triggered, the recorder is already working"),
        {}};
  }

  recorder().start();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ------------------------------ RecorderStopCall -------------------------- */

RecorderStopCall::RecorderStopCall(aegis_proto::Recorder::AsyncService* service,
                                   grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Recorder::AsyncService::RequestStop) {}

RecorderStopCall::~RecorderStopCall() = default;

std::unique_ptr<RecorderStopCallData> RecorderStopCall::clone() const {
  return std::make_unique<RecorderStopCall>(getService(), getQueue());
}

RecorderStopCall::ProcessResult RecorderStopCall::process(
    const Request& request) const {
  if (!recorder().isRecording()) {
    return {
        grpc::Status(
            grpc::StatusCode::INVALID_ARGUMENT,
            "The stop cannot be triggered, the recorder is already stopped"),
        {}};
  }

  recorder().stop();
  return {grpc::Status::OK, google::protobuf::Empty{}};
}

/* ----------------------------- RecorderListenCall ------------------------ */

RecorderListenCall::RecorderListenCall(
    aegis_proto::Recorder::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : StreamCallData(service, queue, CallTag{this},
                     &aegis_proto::Recorder::AsyncService::RequestListen) {}

RecorderListenCall::~RecorderListenCall() = default;

std::unique_ptr<RecorderListenCallData> RecorderListenCall::clone() const {
  return std::make_unique<RecorderListenCall>(getService(), getQueue());
}

RecorderListenCall::ProcessResult RecorderListenCall::process(
    const Request& request) const {
  return grpc::Status::OK;
}

/* ------------------------------- RecorderService -------------------------- */

RecorderService::RecorderService() = default;

RecorderService::~RecorderService() = default;

void RecorderService::start(grpc::ServerCompletionQueue* queue) {
  auto start_call = new RecorderStartCall(this, queue);
  auto stop_call = new RecorderStopCall(this, queue);
  auto listen_call = new RecorderListenCall(this, queue);

  start_call->proceed();
  stop_call->proceed();
  listen_call->proceed();
}

}  // namespace aegis
