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
  Response<> report();

 private:
  State m_state;
  QScopedPointer<Recorder> m_recorder;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_RECORD_H