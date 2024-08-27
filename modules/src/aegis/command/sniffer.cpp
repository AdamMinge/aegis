/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/sniffer.h"

#include "aegis/manager.h"
#include "aegis/sniff/sniffer.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- SnifferCommand --------------------------- */

SnifferCommand::SnifferCommand()
    : Command(QLatin1String("Sniffer")),
      m_state(State::Stopped),
      m_sniffer(new Sniffer) {
  m_parser.addOptions({
      {{"s", "start"}, "Start the Recorder"},
      {{"t", "stop"}, "Stop the Recorder"},
  });
}

SnifferCommand::~SnifferCommand() = default;

QByteArray SnifferCommand::exec() {
  if (m_parser.isSet("start")) return serializer()->serialize(start());
  if (m_parser.isSet("stop")) return serializer()->serialize(stop());

  auto error = Response<>(
      ErrorMessage(getError(), "At least one of options must be provided."));
  return serializer()->serialize(error);
}

Response<> SnifferCommand::start() {
  if (m_state == State::Running) {
    return ErrorMessage(
        getError(),
        QLatin1String(
            "The start cannot be triggered, the sniffer is already working"));
  }

  m_state = State::Running;
  m_sniffer->start();

  return EmptyMessage();
}

Response<> SnifferCommand::stop() {
  if (m_state == State::Stopped) {
    return ErrorMessage(
        getError(),
        QLatin1String(
            "The stop cannot be triggered, the sniffer is already stopped"));
  }

  m_state = State::Stopped;
  m_sniffer->stop();

  return EmptyMessage();
}

}  // namespace aegis