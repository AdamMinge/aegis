#ifndef AEGIS_SNIFF_SNIFFER_H
#define AEGIS_SNIFF_SNIFFER_H

/* ------------------------------------ Qt ---------------------------------- */
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
  void eventAppeared(QObject *obj, QEvent *event);

 protected Q_SLOTS:
  bool eventFilter(QObject *obj, QEvent *event) override;
};

/* ---------------------------------- Sniffer ------------------------------- */

class LIB_AEGIS_API Sniffer : public QObject {
  Q_OBJECT

 public:
  Sniffer(QObject *parent = nullptr);
  ~Sniffer();

  void start();
  void stop();

  [[nodiscard]] bool isSniffing();

 protected Q_SLOTS:
  void onEventAppeared(QObject *obj, QEvent *event);

 private:
  bool m_sniffing;
  QScopedPointer<SnifferListener> m_listener;
};

}  // namespace aegis

#endif  // AEGIS_SNIFF_SNIFFER_H