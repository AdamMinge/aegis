/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/sniffer.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SnifferListener --------------------------- */

SnifferListener::SnifferListener(QObject *parent) : QObject(parent) {}

SnifferListener::~SnifferListener() = default;

bool SnifferListener::eventFilter(QObject *obj, QEvent *event) {
  return QObject::eventFilter(obj, event);
}

/* ---------------------------------- Sniffer ------------------------------- */

Sniffer::Sniffer(QObject *parent)
    : QObject(parent), m_sniffing(false), m_listener(new SnifferListener) {
  connect(m_listener.get(), &SnifferListener::eventAppeared, this,
          &Sniffer::onEventAppeared);
}

Sniffer::~Sniffer() = default;

void Sniffer::start() {
  if (!m_sniffing) {
    QMetaObject::invokeMethod(
        qApp,
        [this]() {
          m_sniffing = true;
          qApp->installEventFilter(m_listener.get());
        },
        Qt::QueuedConnection);
  }
}

void Sniffer::stop() {
  if (m_sniffing) {
    QMetaObject::invokeMethod(
        qApp,
        [this]() {
          m_sniffing = false;
          qApp->removeEventFilter(m_listener.get());
        },
        Qt::QueuedConnection);
  }
}

bool Sniffer::isSniffing() { return m_sniffing; }

void Sniffer::onEventAppeared(QObject *obj, QEvent *event) {}

}  // namespace aegis
