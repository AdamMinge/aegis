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

ObjectObserver::ObjectObserver() : m_observing(false), m_root(nullptr) {}

ObjectObserver::~ObjectObserver() { stop(); }

void ObjectObserver::start() {
  if (m_observing) return;

  QMetaObject::invokeMethod(
    qApp,
    [this]() {
      m_observing = true;

      init();
      qApp->installEventFilter(this);
    },
    Qt::QueuedConnection);
}

void ObjectObserver::stop() {
  if (!m_observing) return;

  QMetaObject::invokeMethod(
    qApp,
    [this]() {
      m_observing = false;

      qApp->removeEventFilter(this);
    },
    Qt::QueuedConnection);
}

bool ObjectObserver::isObserving() const { return m_observing; }

void ObjectObserver::setRoot(QObject *root) { m_root = root; }

bool ObjectObserver::eventFilter(QObject *object, QEvent *event) {
  if (!isObserved(object)) return QObject::eventFilter(object, event);

  switch (event->type()) {
    case QEvent::Create: {
      Q_ASSERT(!m_observed.contains(object));
      m_observed[object] = searcher().getQuery(object);

      const auto &object_query = m_observed[object];
      const auto parent_query =
        m_observed.value(object->parent(), ObjectQuery{});

      Q_EMIT actionReported(
        ObservedAction::ObjectAdded(object_query, parent_query));
      break;
    }

    case QEvent::Destroy: {
      const auto &object_query = m_observed[object];
      const auto parent_query =
        m_observed.value(object->parent(), ObjectQuery{});

      Q_EMIT actionReported(
        ObservedAction::ObjectRemoved(object_query, parent_query));

      Q_ASSERT(m_observed.contains(object));
      m_observed.remove(object);

      break;
    }

    case QEvent::ParentChange: {
      if (!m_observed.contains(object)) break;

      const auto &object_query = m_observed[object];
      const auto parent_query =
        m_observed.value(object->parent(), ObjectQuery{});

      Q_EMIT actionReported(
        ObservedAction::ObjectReparented(object_query, parent_query));
      break;
    }
  }

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

QObjectList ObjectObserver::getRoots() const {
  if (m_root) {
    auto roots = QObjectList{};
    roots.append(const_cast<QObject *>(m_root));
    return roots;
  }

  return getTopLevelObjects();
}

bool ObjectObserver::isObserved(const QObject *object) const {
  if (!m_root) return true;

  while (object) {
    if (object == m_root) return true;
    object = object->parent();
  }

  return false;
}

/* ----------------------------- ObjectObserverQueue ------------------------ */

ObjectObserverQueue::ObjectObserverQueue() : m_observer(nullptr) {}

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
