/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/sniffer.h"

#include "aegis/sniff/widget_marker.h"
#include "aegis/sniff/widget_tooltip.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QMouseEvent>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SnifferListener --------------------------- */

SnifferListener::SnifferListener(QObject *parent)
    : QObject(parent), m_current_widget(nullptr) {}

SnifferListener::~SnifferListener() = default;

bool SnifferListener::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    const auto mouse_event = static_cast<QMouseEvent *>(event);

    const auto global_pos = mouse_event->globalPosition().toPoint();
    const auto widget = qApp->widgetAt(global_pos);

    Q_EMIT mouseMoved(global_pos);

    if (m_current_widget != widget) {
      m_current_widget = widget;
      Q_EMIT currentWidgetChanged(widget);
    }
  }

  return QObject::eventFilter(obj, event);
}

/* ---------------------------------- Sniffer ------------------------------- */

Sniffer::Sniffer(QObject *parent)
    : QObject(parent),
      m_sniffing(false),
      m_tooltip(new SnifferWidgetTooltip),
      m_marker(new SnifferWidgetMarker),
      m_listener(new SnifferListener) {
  connect(m_listener.get(), &SnifferListener::currentWidgetChanged, this,
          &Sniffer::onCurrentWidgetChanged);
  connect(m_listener.get(), &SnifferListener::mouseMoved, this,
          &Sniffer::onMouseMoved);
}

Sniffer::~Sniffer() = default;

void Sniffer::start() {
  if (!m_sniffing) {
    QMetaObject::invokeMethod(
        qApp,
        [this]() {
          m_sniffing = true;

          qApp->installEventFilter(m_listener.get());

          m_tooltip->show();
          m_marker->show();
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

          m_tooltip->hide();
          m_marker->hide();
        },
        Qt::QueuedConnection);
  }
}

bool Sniffer::isSniffing() { return m_sniffing; }

QColor Sniffer::getMarkerColor() const { return m_marker->getColor(); }

void Sniffer::setMarkerColor(QColor color) { m_marker->setColor(color); }

void Sniffer::onCurrentWidgetChanged(QWidget *widget) {
  m_tooltip->setWidget(widget);
  m_marker->setWidget(widget);
}

void Sniffer::onMouseMoved(const QPoint &position) {
  m_tooltip->move(position + QPoint(10, 10));
}

}  // namespace aegis
