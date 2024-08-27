#ifndef AEGIS_SNIFFER_H
#define AEGIS_SNIFFER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QMouseEvent>
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------------- Sniffer ------------------------------- */

class SnifferWidgetTooltip;
class SnifferWidgetMarker;

class SnifferPrivate;

class LIB_AEGIS_API Sniffer : public QObject {
  Q_OBJECT

 public:
  Sniffer(QObject *parent = nullptr);
  ~Sniffer() override;

  void start();
  void stop();

  [[nodiscard]] bool isSniffing();

  [[nodiscard]] QColor getMarkerColor() const;
  void setMarkerColor(QColor color);

 protected Q_SLOTS:
  bool eventFilter(QObject *obj, QEvent *event) override;
  void onMouseMove(QMouseEvent *event);

 private:
  bool m_sniffing;
  QScopedPointer<SnifferWidgetTooltip> m_tooltip;
  QScopedPointer<SnifferWidgetMarker> m_marker;
};

}  // namespace aegis

#endif  // AEGIS_SNIFFER_H