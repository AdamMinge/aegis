/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/command.h"

#include "aegis/manager.h"
#include "aegis/response.h"
#include "aegis/serializer.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QCommandLineParser>
/* -------------------------------------------------------------------------- */

namespace aegis {

Command::Command(const QString& name) : m_name(name) {
  m_parser.addOptions({{{"h", "help"}, "Displays help for command."}});
}

Command::~Command() = default;

QString Command::getName() const { return m_name; }

QString Command::getError() const {
  return QLatin1String("%1 Command Error").arg(m_name);
}

QString Command::getHelp() const {
  auto help = m_parser.helpText();
  help.replace(qApp->arguments().constFirst(), m_name);

  return help;
}

QByteArray Command::exec(const QStringList& args) {
  if (!m_parser.parse(args)) {
    auto error = Response<>(ErrorMessage(getError(), m_parser.errorText()));
    return serializer()->serialize(error);
  }

  if (m_parser.isSet("help")) {
    auto help = Response<HelpMessage>(HelpMessage(getHelp()));
    return serializer()->serialize(help);
  }

  for (auto required_option : m_required_options) {
    if (!m_parser.isSet(required_option)) {
      auto error = Response<>(ErrorMessage(
          getError(),
          QLatin1String("Option '%1' must be provided.").arg(required_option)));
      return serializer()->serialize(error);
    }
  }

  return exec();
}

}  // namespace aegis