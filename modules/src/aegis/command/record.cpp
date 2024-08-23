/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/record.h"

#include "aegis/manager.h"
#include "aegis/record/action.h"
#include "aegis/record/recorder.h"
#include "aegis/record/strategy.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- RecordCommand ---------------------------- */

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

/* ------------------------------- RecordCommand ---------------------------- */

RecordCommand::RecordCommand()
    : Command(QLatin1String("Record")),
      m_state(State::Stopped),
      m_recorder(new Recorder) {
  m_parser.addOptions({
      {{"s", "start"}, "Start the Recorder"},
      {{"p", "pause"}, "Pause the Recorder"},
      {{"t", "stop"}, "Stop the Recorder"},
      {{"r", "report"}, "Report the Recorder"},
  });

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

RecordCommand::~RecordCommand() = default;

QByteArray RecordCommand::exec() {
  if (m_parser.isSet("start")) return serializer()->serialize(start());
  if (m_parser.isSet("pause")) return serializer()->serialize(pause());
  if (m_parser.isSet("stop")) return serializer()->serialize(stop());
  if (m_parser.isSet("report")) return serializer()->serialize(report());

  auto error = Response<>(
      ErrorMessage(getError(), "At least one of options must be provided."));
  return serializer()->serialize(error);
}

Response<> RecordCommand::start() {
  if (m_state == State::Running) {
    return ErrorMessage(
        getError(),
        QLatin1String(
            "The start cannot be triggered, the recorder is already working"));
  }

  m_state = State::Running;
  m_recorder->start();

  return EmptyMessage();
}

Response<> RecordCommand::pause() {
  if (m_state != State::Running) {
    return ErrorMessage(
        getError(),
        QLatin1String(
            "The pause cannot be triggered, the recorder is not working"));
  }

  m_state = State::Paused;
  m_recorder->stop();

  return EmptyMessage();
}

Response<> RecordCommand::stop() {
  if (m_state == State::Stopped) {
    return ErrorMessage(
        getError(),
        QLatin1String(
            "The stop cannot be triggered, the recorder is already stopped"));
  }

  m_state = State::Stopped;
  m_recorder->stop();
  m_recorder->clearRecordedActions();

  return EmptyMessage();
}

Response<ReportMessage> RecordCommand::report() {
  const auto actions_mapper = RecordedActionsMapper{};
  const auto recorded_actions = m_recorder->getRecordedActions();

  auto message = ReportMessage{};
  for (const auto& action : recorded_actions) {
    const auto command = action.visit(actions_mapper);
    message.commands.append(command);
  }

  return message;
}

}  // namespace aegis
