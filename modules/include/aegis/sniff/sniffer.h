#ifndef AEGIS_SNIFF_SNIFFER_H
#define AEGIS_SNIFF_SNIFFER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QMouseEvent>
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------- SnifferWidgetListener ------------------------ */

class LIB_AEGIS_API SnifferWidgetListener : public QObject {
  Q_OBJECT

 public:
  explicit SnifferWidgetListener(QObject *parent = nullptr);
  ~SnifferWidgetListener() override;

 Q_SIGNALS:
  void currentWidgetChanged(QWidget *widget);

 protected Q_SLOTS:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  void setWidget(QWidget *widget);

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

 private:
  bool m_sniffing;
  QScopedPointer<SnifferWidgetTooltip> m_tooltip;
  QScopedPointer<SnifferWidgetMarker> m_marker;
  QScopedPointer<SnifferWidgetListener> m_widget_listener;
};

}  // namespace aegis

#endif  // AEGIS_SNIFF_SNIFFER_H