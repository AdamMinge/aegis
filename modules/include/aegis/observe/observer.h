#ifndef AEGIS_OBSERVE_OBSERVER_H
#define AEGIS_OBSERVE_OBSERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QHash>
#include <QObject>
/* ---------------------------------- Standard ------------------------------ */
#include <condition_variable>
#include <mutex>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/observe/action.h"
#include "aegis/search/query.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- ObjectObserver --------------------------- */

class LIB_AEGIS_API ObjectObserver : public QObject {
  Q_OBJECT

public:
  explicit ObjectObserver();
  ~ObjectObserver() override;

  void start();
  void stop();

  [[nodiscard]] bool isObserving() const;

  void setRoot(QObject *root);

Q_SIGNALS:
  void actionReported(const ObservedAction &action);

protected:
  [[nodiscard]] bool eventFilter(QObject *object, QEvent *event) override;

private:
  [[nodiscard]] QObjectList getRoots() const;
  [[nodiscard]] bool isObserved(const QObject *object) const;

private:
  bool m_observing;
  QObject *m_root;
};

/* ----------------------------- ObjectObserverQueue ------------------------ */

class LIB_AEGIS_API ObjectObserverQueue {
public:
  explicit ObjectObserverQueue();
  ~ObjectObserverQueue();

  void setObserver(ObjectObserver *observer);

  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] ObservedAction popAction();
  [[nodiscard]] ObservedAction waitPopAction();

private:
  ObjectObserver *m_observer;
  QMetaObject::Connection m_on_action_reported;
  QQueue<ObservedAction> m_observed_actions;

  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};

}// namespace aegis

#endif// AEGIS_OBSERVE_OBSERVER_H