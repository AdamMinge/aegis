#ifndef AEGIS_RECORD_RECORDER_H
#define AEGIS_RECORD_RECORDER_H

/* --------------------------------- Standard ------------------------------- */
#include <unordered_map>
/* ------------------------------------ Qt ---------------------------------- */
#include <QQueue>
#include <QScopedPointer>
#include <QThread>
#include <QWidget>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class RecordStrategy;
class RecordedAction;

/* --------------------------- RecorderWidgetListener ----------------------- */

class LIB_AEGIS_API RecorderWidgetListener : public QObject {
  Q_OBJECT

 public:
  explicit RecorderWidgetListener(QObject *parent = nullptr);
  ~RecorderWidgetListener() override;

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
  ~Recorder();

  void start();
  void stop();

  [[nodiscard]] bool isRecording() const;

  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] RecordedAction popAction();

  bool addStrategy(std::unique_ptr<RecordStrategy> &&strategy);

 protected Q_SLOTS:
  void onCurrentWidgetChanged(QWidget *widget);
  void onActionReported(const RecordedAction &action);

 private:
  [[nodiscard]] RecordStrategy *findStrategy(QWidget *widget) const;

 private:
  std::unordered_map<int, std::unique_ptr<RecordStrategy>> m_strategies;
  QQueue<RecordedAction> m_recorded_actions;
  RecordStrategy *m_current_strategy;
  QScopedPointer<RecorderWidgetListener> m_widget_listener;
  bool m_running;
};

}  // namespace aegis

#endif  // AEGIS_RECORD_RECORDER_H