#ifndef AEGIS_COMMAND_SNIFFER_H
#define AEGIS_COMMAND_SNIFFER_H

/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"
#include "aegis/export.h"
#include "aegis/response.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;
class Sniffer;

/* ------------------------------- SnifferCommand --------------------------- */

class LIB_AEGIS_API SnifferCommand : public Command {
 public:
  enum class State { Running, Stopped };

 public:
  explicit SnifferCommand();
  ~SnifferCommand() override;

 protected:
  [[nodiscard]] QByteArray exec() override;

 private:
  Response<> start();
  Response<> stop();

 private:
  State m_state;
  QScopedPointer<Sniffer> m_sniffer;
};

}  // namespace aegis

#endif  // AEGIS_COMMAND_SNIFFER_H
