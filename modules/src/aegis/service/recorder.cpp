/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/recorder.h"

#include "aegis/module.h"
#include "aegis/record/action.h"
#include "aegis/record/recorder.h"
#include "aegis/record/strategy.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------- RecordedActionsMapper ------------------------ */

class RecordedActionsMapper {
public:
  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ContextMenuOpened &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ContextMenuOpened %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ButtonClicked &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ButtonClicked %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ButtonToggled &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ButtonToggled %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ComboBoxCurrentChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ComboBoxCurrentChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::SpinBoxValueChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"SpinBoxValueChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::DoubleSpinBoxValueChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"DoubleSpinBoxValueChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::SliderValueChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"SliderValueChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::TabCurrentChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"TabCurrentChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::TabClosed &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"TabClosed %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::TabMoved &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(
      QLatin1String{"TabMoved %1"}.arg(action.object.toString()).toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ToolBoxCurrentChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ToolBoxCurrentChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::ActionTriggered &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"ActionTriggered %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::TextEditTextChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"TextEditTextChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::LineEditTextChanged &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"LineEditTextChanged %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }

  aegis_proto::RecorderListenResponse
  operator()(const RecordedAction::LineEditReturnPressed &action) const {
    auto response = aegis_proto::RecorderListenResponse{};
    response.set_command(QLatin1String{"LineEditReturnPressed %1"}
                           .arg(action.object.toString())
                           .toStdString());
    return response;
  }
};

/* ----------------------------- RecorderListenCall ------------------------ */

RecorderListenCall::RecorderListenCall(
  aegis_proto::Recorder::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : StreamCallData(
        service, queue, CallTag{this},
        &aegis_proto::Recorder::AsyncService::RequestListen),
      m_recorder(std::make_unique<ActionRecorder>()),
      m_recorder_queue(std::make_unique<ActionRecorderQueue>()),
      m_mapper(std::make_unique<RecordedActionsMapper>()) {
  m_recorder->addStrategy(new ActionRecordWidgetStrategy());
  m_recorder->addStrategy(new ActionRecordButtonStrategy());
  m_recorder->addStrategy(new ActionRecordComboBoxStrategy());
  m_recorder->addStrategy(new ActionRecordSpinBoxStrategy());
  m_recorder->addStrategy(new ActionRecordSliderStrategy());
  m_recorder->addStrategy(new ActionRecordTabBarStrategy());
  m_recorder->addStrategy(new ActionRecordToolBoxStrategy());
  m_recorder->addStrategy(new ActionRecordMenuStrategy());
  m_recorder->addStrategy(new ActionRecordTextEditStrategy());
  m_recorder->addStrategy(new ActionRecordLineEditStrategy());
  m_recorder->addStrategy(new ActionRecordItemViewStrategy());
  m_recorder->addStrategy(new ActionRecordButtonStrategy());

  m_recorder_queue->setRecorder(m_recorder.get());
  m_recorder->moveToThread(qApp->thread());
  m_recorder->start();
}

RecorderListenCall::~RecorderListenCall() = default;

std::unique_ptr<RecorderListenCallData> RecorderListenCall::clone() const {
  return std::make_unique<RecorderListenCall>(getService(), getQueue());
}

RecorderListenCall::ProcessResult
RecorderListenCall::process(const Request &request) const {
  if (m_recorder_queue->isEmpty()) return {};

  const auto recorded_action = m_recorder_queue->popAction();
  const auto response = recorded_action.visit(*m_mapper);

  return response;
}

/* ------------------------------- RecorderService -------------------------- */

RecorderService::RecorderService() = default;

RecorderService::~RecorderService() = default;

void RecorderService::start(grpc::ServerCompletionQueue *queue) {
  auto listen_call = new RecorderListenCall(this, queue);
  listen_call->proceed();
}

}// namespace aegis
