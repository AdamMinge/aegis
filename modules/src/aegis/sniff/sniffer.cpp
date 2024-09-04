/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/sniffer.h"

#include "aegis/sniff/widget_marker.h"
#include "aegis/sniff/widget_tooltip.h"
/* ----------------------------------- Qtils -------------------------------- */
#include <qtils/dpi/dpi.h>
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QMouseEvent>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------------- Sniffer ------------------------------- */

Sniffer::Sniffer(QObject *parent)
    : QObject(parent),
      m_sniffing(false),
      m_tooltip(new SnifferWidgetTooltip),
      m_marker(new SnifferWidgetMarker) {}

Sniffer::~Sniffer() = default;

void Sniffer::start() {
  if (!m_sniffing) {
    m_sniffing = true;

    QMetaObject::invokeMethod(
        qApp,
        [this]() {
          qApp->installEventFilter(this);

          m_tooltip->show();
          m_marker->show();
        },
        Qt::QueuedConnection);
  }
}

void Sniffer::stop() {
  if (m_sniffing) {
    m_sniffing = false;

    QMetaObject::invokeMethod(
        qApp,
        [this]() {
          qApp->removeEventFilter(this);

          m_tooltip->hide();
          m_marker->hide();
        },
        Qt::QueuedConnection);
  }
}

bool Sniffer::isSniffing() { return m_sniffing; }

QColor Sniffer::getMarkerColor() const { return m_marker->getColor(); }

void Sniffer::setMarkerColor(QColor color) { m_marker->setColor(color); }

bool Sniffer::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    onMouseMove(static_cast<QMouseEvent *>(event));
  }

  return QObject::eventFilter(obj, event);
}

void Sniffer::onMouseMove(QMouseEvent *event) {
  const auto global_pos = event->globalPosition().toPoint();
  const auto widget = qApp->widgetAt(global_pos);

  m_tooltip->move(global_pos + qtils::dpiScaled(QPoint(10, 10)));
  m_tooltip->setWidget(widget);
  m_marker->setWidget(widget);
}

}  // namespace aegis
