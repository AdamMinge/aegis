#ifndef AEGIS_COMMAND_RECORD_H
#define AEGIS_COMMAND_RECORD_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class Recorder;

/* -------------------------------- ReportMessage --------------------------- */

struct LIB_AEGIS_API ReportMessage {
  Q_GADGET

 public:
  Q_PROPERTY(QStringList commands MEMBER commands)

  QStringList commands;
};

/* ------------------------------- RecordCommand ---------------------------- */

class LIB_AEGIS_API RecordCommand : public Command {
 public:
  enum class State { Running, Paused, Stopped };

 public:
  explicit RecordCommand();
  ~RecordCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  Response<> start();
  Response<> pause();
  Response<> stop();
  Response<ReportMessage> report();

 private:
  State m_state;
  QScopedPointer<Recorder> m_recorder;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_RECORD_H