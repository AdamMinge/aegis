/* ----------------------------------- Local -------------------------------- */
#include "aegis/observe/observer.h"

#include "aegis/module.h"
#include "aegis/search/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- ObjectObserver --------------------------- */

ObjectObserver::ObjectObserver() = default;

ObjectObserver::~ObjectObserver() = default;

void ObjectObserver::start() {
  qApp->installEventFilter(this);
  init();
}

void ObjectObserver::stop() { qApp->removeEventFilter(this); }

void ObjectObserver::setRoot(QObject *root) { m_root = root; }

bool ObjectObserver::eventFilter(QObject *object, QEvent *event) {
  if (isObserved(object)) {
    const auto object_query = m_observed[object];
    const auto parent_query = m_observed.value(object->parent(), ObjectQuery{});

    switch (event->type()) {
      case QEvent::Destroy:
        Q_EMIT actionReported(
          ObservedAction::ObjectDestroyed(object_query, parent_query));
        break;
      case QEvent::Create:
        Q_EMIT actionReported(
          ObservedAction::ObjectCreated(object_query, parent_query));
        break;
      case QEvent::ParentChange:
        Q_EMIT actionReported(
          ObservedAction::ObjectReparented(object_query, parent_query));
        break;
    }
  }

  // connect inside of strategy and return connection object to disconnect after all
  // move lambda into installed event filter and emit in inside of it

  return QObject::eventFilter(object, event);
}

void ObjectObserver::init() {
  m_observed.clear();

  auto objectsToProcess = std::queue<QObject *>{};
  for (const auto filter : getRoots()) { objectsToProcess.push(filter); }

  while (!objectsToProcess.empty()) {
    auto object = objectsToProcess.front();
    objectsToProcess.pop();

    m_observed[object] = searcher().getQuery(object);

    for (const auto child : object->children()) {
      objectsToProcess.push(child);
    }
  }
}

bool ObjectObserver::isObserved(const QObject *object) const {
  if (!m_root) return true;

  while (object) {
    if (object == m_root) return true;
    object = object->parent();
  }

  return false;
}

QObjectList ObjectObserver::getRoots() const {
  if (m_root) {
    auto roots = QObjectList{};
    roots.append(const_cast<QObject *>(m_root));
    return roots;
  }

  return getTopLevelObjects();
}

/* ----------------------------- ObjectObserverQueue ------------------------ */

ObjectObserverQueue::ObjectObserverQueue() = default;

ObjectObserverQueue::~ObjectObserverQueue() = default;

void ObjectObserverQueue::setObserver(ObjectObserver *observer) {
  if (m_observer) {
    m_observer->disconnect(m_on_action_reported);
    m_observed_actions.clear();
  }

  m_observer = observer;

  if (m_observer) {
    m_on_action_reported = QObject::connect(
      m_observer, &ObjectObserver::actionReported,
      [this](const auto recorder_action) {
        m_observed_actions.push_back(recorder_action);
      });
  }
}

bool ObjectObserverQueue::isEmpty() const { return m_observed_actions.empty(); }

ObservedAction ObjectObserverQueue::popAction() {
  return m_observed_actions.takeFirst();
}

}// namespace aegis
