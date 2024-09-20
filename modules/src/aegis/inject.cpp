/* ----------------------------------- Local -------------------------------- */
#include "aegis/config.h"
#include "aegis/module.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QtGlobal>
/* ---------------------------------- Standard ------------------------------ */
#include <thread>
/* -------------------------------------------------------------------------- */

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
      std::thread{[host, port]() { aegis::server().listen(host, port); }};
  server_thread.detach();
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
