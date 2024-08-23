/* ----------------------------------- Local -------------------------------- */
#include "application.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QCommandLineParser>
/* ----------------------------------- Aegis -------------------------------- */
#include <aegis/manager.h>
/* -------------------------------------------------------------------------- */

/* -------------------------- BaseCommandLineParser ------------------------- */

class BaseCommandLineParser {
 public:
  explicit BaseCommandLineParser();
  ~BaseCommandLineParser();

  void process(const QCoreApplication &app);

  [[nodiscard]] quint16 getServerPort() const;
  [[nodiscard]] bool enableSniffer() const;

 private:
  quint16 m_server_port;
  bool m_enable_sniffer;
};

BaseCommandLineParser::BaseCommandLineParser() {}

BaseCommandLineParser::~BaseCommandLineParser() = default;

void BaseCommandLineParser::process(const QCoreApplication &app) {
  QCommandLineParser parser;

  auto port_option = QCommandLineOption(
      {"p", "port"}, "Port aegis server will listen to.", "port");
  port_option.setDefaultValue("5010");
  parser.addOption(port_option);

  auto sniffer_option =
      QCommandLineOption({"s", "sniffer"}, "Enable aegis sniffer.");
  parser.addOption(sniffer_option);

  parser.process(app);

  m_server_port = parser.value(port_option).toUInt();
  m_enable_sniffer = parser.isSet(sniffer_option);
}

quint16 BaseCommandLineParser::getServerPort() const { return m_server_port; }

bool BaseCommandLineParser::enableSniffer() const { return m_enable_sniffer; }

/* -------------------------------- Application ----------------------------- */

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
  setApplicationName(QStringLiteral("Example-Server"));
  setOrganizationName(QStringLiteral("Aegis"));

  parseCommandLine();
}

Application::~Application() = default;

void Application::parseCommandLine() {
  BaseCommandLineParser parser;
  parser.process(*this);

  const auto server_port = parser.getServerPort();
  const auto enable_sniffer = parser.enableSniffer();

  aegis::server()->listen(QHostAddress::LocalHost, server_port);
  if (enable_sniffer) aegis::sniffer()->start();
}
