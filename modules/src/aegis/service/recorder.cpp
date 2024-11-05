/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/recorder.h"

#include "aegis/module.h"
#include "aegis/record/action.h"
#include "aegis/record/recorder.h"
#include "aegis/record/strategy.h"
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

/* ----------------------------- RecorderListenCall ------------------------ */

RecorderListenCall::RecorderListenCall(
    aegis_proto::Recorder::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : StreamCallData(service, queue, CallTag{this},
                     &aegis_proto::Recorder::AsyncService::RequestListen),
      m_recorder(std::make_unique<Recorder>()),
      m_mapper(std::make_unique<RecordedActionsMapper>()) {
  m_recorder->addStrategy(std::make_unique<RecordWidgetStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordButtonStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordComboBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordSpinBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordSliderStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordTabBarStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordToolBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordMenuStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordTextEditStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordLineEditStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordItemViewStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordButtonStrategy>());
}

RecorderListenCall::~RecorderListenCall() = default;

std::unique_ptr<RecorderListenCallData> RecorderListenCall::clone() const {
  return std::make_unique<RecorderListenCall>(getService(), getQueue());
}

RecorderListenCall::ProcessResult RecorderListenCall::process(
    const Request& request) const {
  if (m_recorder->isEmpty()) return {};

  const auto recorded_action = m_recorder->popAction();
  const auto command = recorded_action.visit(*m_mapper);

  auto response = aegis_proto::RecorderListenResponse{};
  response.set_command(command.toStdString());

  return response;
}

/* ------------------------------- RecorderService -------------------------- */

RecorderService::RecorderService() = default;

RecorderService::~RecorderService() = default;

void RecorderService::start(grpc::ServerCompletionQueue* queue) {
  auto listen_call = new RecorderListenCall(this, queue);
  listen_call->proceed();
}

}  // namespace aegis
