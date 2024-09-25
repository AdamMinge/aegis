#ifndef AEGIS_SNIFF_SNIFFER_H
#define AEGIS_SNIFF_SNIFFER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QMouseEvent>
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SnifferListener --------------------------- */

class LIB_AEGIS_API SnifferListener : public QObject {
  Q_OBJECT

 public:
  explicit SnifferListener(QObject *parent = nullptr);
  ~SnifferListener() override;

 Q_SIGNALS:
  void currentWidgetChanged(QWidget *widget);
  void mouseMoved(const QPoint &position);

 protected Q_SLOTS:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  QWidget *m_current_widget;
};

/* ---------------------------------- Sniffer ------------------------------- */

class SnifferWidgetTooltip;
class SnifferWidgetMarker;

class LIB_AEGIS_API Sniffer : public QObject {
  Q_OBJECT

 public:
  Sniffer(QObject *parent = nullptr);
  ~Sniffer();

  void start();
  void stop();

  [[nodiscard]] bool isSniffing();

  [[nodiscard]] QColor getMarkerColor() const;
  void setMarkerColor(QColor color);

 protected Q_SLOTS:
  void onCurrentWidgetChanged(QWidget *widget);
  void onMouseMoved(const QPoint &position);

 private:
  bool m_sniffing;
  QScopedPointer<SnifferWidgetTooltip> m_tooltip;
  QScopedPointer<SnifferWidgetMarker> m_marker;
  QScopedPointer<SnifferListener> m_listener;
};

}  // namespace aegis

#endif  // AEGIS_SNIFF_SNIFFER_H