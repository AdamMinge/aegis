#ifndef AEGIS_RECORD_RECORDER_H
#define AEGIS_RECORD_RECORDER_H

/* --------------------------------- Standard ------------------------------- */
#include <unordered_map>
/* ------------------------------------ Qt ---------------------------------- */
#include <QThread>
#include <QWidget>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class RecordStrategy;

/* ------------------------------- WidgetListener --------------------------- */

class LIB_AEGIS_API WidgetListener : public QObject {
  Q_OBJECT

 public:
  explicit WidgetListener(QObject *parent = nullptr);
  ~WidgetListener() override;

 Q_SIGNALS:
  void currentWidgetChanged(QWidget *widget);

 protected Q_SLOTS:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  void setWidget(QWidget *widget);
  [[nodiscard]] QWidget *findWidget(QWidget *widget) const;

 private:
  QWidget *m_current_widget;
};

/* ---------------------------------- Recorder ------------------------------ */

class LIB_AEGIS_API Recorder : public QObject {
  Q_OBJECT

 public:
  explicit Recorder(QObject *parent = nullptr);
  ~Recorder() override;

  void start();
  void stop();
  void clear();

  [[nodiscard]] const QStringList &getReport() const;

  bool addStrategy(std::unique_ptr<RecordStrategy> &&strategy);
  std::unique_ptr<RecordStrategy> takeStrategy(int type);
  bool removeStrategy(int type);

 protected Q_SLOTS:
  void onCurrentWidgetChanged(QWidget *widget);
  void onRecorder(const QString &command);

 private:
  [[nodiscard]] RecordStrategy *findStrategy(QWidget *widget) const;

 private:
  std::unordered_map<int, std::unique_ptr<RecordStrategy>> m_strategies;
  RecordStrategy *m_current_strategy;
  WidgetListener *m_widget_listener;
  QStringList m_report;
  bool m_running;
};

}  // namespace aegis

#endif  // AEGIS_RECORD_RECORDER_H