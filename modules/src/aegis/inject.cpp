/* ----------------------------------- Local -------------------------------- */
#include "aegis/config.h"
#include "aegis/module.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QThread>
#include <QtGlobal>
/* -------------------------------------------------------------------------- */

class ServerThread : public QThread {
 public:
  ServerThread(aegis::AegisModule& aegis, const QHostAddress& host,
               quint16 port)
      : m_aegis(aegis), m_host(host), m_port(port) {}

 protected:
  void run() override { m_aegis.getServer().listen(m_host, m_port); }

 private:
  aegis::AegisModule& m_aegis;
  QHostAddress m_host;
  quint16 m_port;
};

void startServer() {
  auto valid_port = false;
  auto valid_host = false;

  const auto str_host = qEnvironmentVariable("AEGIS_SERVER_HOST", "0.0.0.0");
  const auto str_port = qEnvironmentVariable("AEGIS_SERVER_PORT", "5010");

  const auto host = QHostAddress(str_host);
  valid_host = !host.isNull();

  const auto port = str_port.toUInt(&valid_port);

  if (!valid_host) return;
  if (!valid_port) return;

  auto server_thread =
      new ServerThread(aegis::AegisModule::getInstance(), host, port);
  server_thread->start();
}

#if defined(AEGIS_OS_WINDOWS)
#include <io.h>
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
    startServer();
  }
  return TRUE;
}
#elif defined(AEGIS_OS_LINUX)
__attribute__((constructor)) static void init(void) { startServer(); }
#else
#error This operating system does not support DLL injection
#endif
