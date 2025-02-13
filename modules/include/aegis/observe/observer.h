#ifndef AEGIS_OBSERVE_OBSERVER_H
#define AEGIS_OBSERVE_OBSERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QHash>
#include <QObject>
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

  void setRoot(QObject *root);

Q_SIGNALS:
  void actionReported(const ObservedAction &action);

protected:
  [[nodiscard]] bool eventFilter(QObject *object, QEvent *event) override;

private:
  void init();
  [[nodiscard]] bool isObserved(const QObject *object) const;
  [[nodiscard]] QObjectList getRoots() const;

private:
  QObject *m_root;
  QHash<QObject *, ObjectQuery> m_observed;
};

/* ----------------------------- ObjectObserverQueue ------------------------ */

class LIB_AEGIS_API ObjectObserverQueue {
public:
  explicit ObjectObserverQueue();
  ~ObjectObserverQueue();

  void setObserver(ObjectObserver *observer);

  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] ObservedAction popAction();

private:
  ObjectObserver *m_observer;
  QMetaObject::Connection m_on_action_reported;
  QQueue<ObservedAction> m_observed_actions;
};

}// namespace aegis

#endif// AEGIS_OBSERVE_OBSERVER_H