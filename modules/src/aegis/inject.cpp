
#include <QCoreApplication>

#include "aegis/manager.h"
#include "config.h"

void startServer() {
  auto valid_port = false;
  auto valid_host = false;

  const auto str_host = qEnvironmentVariable("AEGIS_HOST", "0.0.0.0");
  const auto str_port = qEnvironmentVariable("AEGIS_PORT", "4322");

  const auto host = QHostAddress(str_host);
  valid_host = !host.isNull();

  const auto port = str_port.toUInt(&valid_port);

  if (!valid_host) {
    qDebug() << QLatin1String(
                    "AEGIS_HOST environment variable is incorrect: %1")
                    .arg(str_host);
    return;
  }

  if (!valid_port) {
    qDebug() << QLatin1String(
                    "AEGIS_PORT environment variable is incorrect: %1")
                    .arg(str_port);
    return;
  }

  qDebug() << QLatin1String("Aegis server start listen(%1, %2)")
                  .arg(host.toString())
                  .arg(port);
  aegis::server()->listen(host, port);
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
