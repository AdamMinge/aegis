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
#include "aegis/record/action.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ActionRecordStrategy;

/* ------------------------ ActionRecorderWidgetListener -------------------- */

class LIB_AEGIS_API ActionRecorderWidgetListener : public QObject {
  Q_OBJECT

public:
  explicit ActionRecorderWidgetListener(QObject *parent = nullptr);
  ~ActionRecorderWidgetListener() override;

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

/* ------------------------------- ActionRecorder --------------------------- */

class LIB_AEGIS_API ActionRecorder : public QObject {
  Q_OBJECT

public:
  explicit ActionRecorder(QObject *parent = nullptr);
  ~ActionRecorder();

  void start();
  void stop();

  [[nodiscard]] bool isRecording() const;

  bool addStrategy(std::unique_ptr<ActionRecordStrategy> &&strategy);

Q_SIGNALS:
  void actionReported(const RecordedAction &action);

protected Q_SLOTS:
  void onCurrentWidgetChanged(QWidget *widget);

private:
  [[nodiscard]] ActionRecordStrategy *findStrategy(QWidget *widget) const;

private:
  std::unordered_map<int, std::unique_ptr<ActionRecordStrategy>> m_strategies;
  ActionRecordStrategy *m_current_strategy;
  QScopedPointer<ActionRecorderWidgetListener> m_widget_listener;
  bool m_running;
};

/* ----------------------------- ActionRecorderQueue ------------------------ */

class LIB_AEGIS_API ActionRecorderQueue {
public:
  explicit ActionRecorderQueue();
  ~ActionRecorderQueue();

  void setRecorder(ActionRecorder *recorder);

  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] RecordedAction popAction();

private:
  ActionRecorder *m_recorder;
  QMetaObject::Connection m_on_action_reported;
  QQueue<RecordedAction> m_recorded_actions;
};

}// namespace aegis

#endif// AEGIS_RECORD_RECORDER_H