/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/widget_marker.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QPainter>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- SnifferWidgetMarker ------------------------ */

SnifferWidgetMarker::SnifferWidgetMarker(QWidget *parent)
    : QLabel(parent), m_widget(nullptr), m_color(Qt::red) {
  setVisible(false);
  setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                 Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus |
                 Qt::WindowStaysOnTopHint | Qt::ToolTip);

  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_TransparentForMouseEvents);
}

SnifferWidgetMarker::~SnifferWidgetMarker() = default;

QWidget *SnifferWidgetMarker::getWidget() const { return m_widget; }

void SnifferWidgetMarker::setWidget(QWidget *widget) {
  if (m_widget != widget) {
    m_widget = widget;

    if (m_widget) {
      resize(m_widget->size());
      move(m_widget->mapToGlobal(QPoint(0, 0)));
      show();
      raise();
    } else {
      hide();
    }
  }
}

QColor SnifferWidgetMarker::getColor() const { return m_color; }

void SnifferWidgetMarker::setColor(QColor color) { m_color = color; }

void SnifferWidgetMarker::paintEvent(QPaintEvent *event) {
  if (!m_widget) return;

  auto painter = QPainter(this);
  auto pen = QPen(m_color, 2);

  painter.setPen(pen);
  painter.drawRect(rect().adjusted(1, 1, -2, -2));
}

}  // namespace aegis
