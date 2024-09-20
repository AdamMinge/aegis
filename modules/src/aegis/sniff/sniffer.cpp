/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/sniffer.h"

#include "aegis/sniff/widget_marker.h"
#include "aegis/sniff/widget_tooltip.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------- SnifferWidgetListener ------------------------ */

SnifferWidgetListener::SnifferWidgetListener(QObject *parent)
    : QObject(parent), m_current_widget(nullptr) {}

SnifferWidgetListener::~SnifferWidgetListener() = default;

bool SnifferWidgetListener::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    const auto mouse_event = static_cast<QMouseEvent *>(event);

    const auto global_pos = mouse_event->globalPosition().toPoint();
    const auto widget = qApp->widgetAt(global_pos);

    setWidget(widget);
  }

  return QObject::eventFilter(obj, event);
}

void SnifferWidgetListener::setWidget(QWidget *widget) {
  if (m_current_widget != widget) {
    m_current_widget = widget;
    Q_EMIT currentWidgetChanged(widget);
  }
}

/* ---------------------------------- Sniffer ------------------------------- */

Sniffer::Sniffer(QObject *parent)
    : QObject(parent),
      m_sniffing(false),
      m_tooltip(new SnifferWidgetTooltip),
      m_marker(new SnifferWidgetMarker),
      m_widget_listener(new SnifferWidgetListener) {
  connect(m_widget_listener.get(), &SnifferWidgetListener::currentWidgetChanged,
          this, &Sniffer::onCurrentWidgetChanged);
}

Sniffer::~Sniffer() = default;

void Sniffer::start() {
  if (!m_sniffing) {
    m_sniffing = true;

    qApp->installEventFilter(m_widget_listener.get());

    m_tooltip->show();
    m_marker->show();
  }
}

void Sniffer::stop() {
  if (m_sniffing) {
    m_sniffing = false;

    qApp->removeEventFilter(m_widget_listener.get());

    m_tooltip->hide();
    m_marker->hide();
  }
}

bool Sniffer::isSniffing() { return m_sniffing; }

QColor Sniffer::getMarkerColor() const { return m_marker->getColor(); }

void Sniffer::setMarkerColor(QColor color) { m_marker->setColor(color); }

void Sniffer::onCurrentWidgetChanged(QWidget *widget) {
  const auto cursor_pos = QCursor::pos();
  m_tooltip->move(cursor_pos + QPoint(10, 10));
  m_tooltip->setWidget(widget);
  m_marker->setWidget(widget);
}

}  // namespace aegis
