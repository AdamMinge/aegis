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

ObjectObserver::ObjectObserver()
    : m_observing(false), m_root(nullptr), m_check_timer(new QTimer(this)) {
  m_check_timer->setInterval(100);
  connect(
    m_check_timer, &QTimer::timeout, this, &ObjectObserver::checkForRenames);
}

ObjectObserver::~ObjectObserver() { stop(); }

void ObjectObserver::start() {
  if (m_observing) return;


  QMetaObject::invokeMethod(
    qApp,
    [this]() {
      m_observing = true;
      startRenameTracker();
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
      stopRenameTracker();
      qApp->removeEventFilter(this);
    },
    Qt::QueuedConnection);
}

bool ObjectObserver::isObserving() const { return m_observing; }

void ObjectObserver::setRoot(QObject *root) { m_root = root; }

bool ObjectObserver::eventFilter(QObject *object, QEvent *event) {
  if (!isObserved(object)) return QObject::eventFilter(object, event);

  auto object_query = searcher().getQuery(object);
  auto parent_query = searcher().getQuery(object->parent());

  switch (event->type()) {
    case QEvent::Create: {
      Q_EMIT actionReported(
        ObservedAction::ObjectAdded{object_query, parent_query});

      std::lock_guard<std::mutex> lock(m_mutex);
      m_tracked_objects.insert(object, object_query);
      break;
    }

    case QEvent::Destroy: {
      Q_EMIT actionReported(ObservedAction::ObjectRemoved{object_query});

      std::lock_guard<std::mutex> lock(m_mutex);
      m_tracked_objects.remove(object);
      break;
    }

    case QEvent::ParentChange: {
      Q_EMIT actionReported(
        ObservedAction::ObjectReparented{object_query, parent_query});
      break;
    }
  }

  return QObject::eventFilter(object, event);
}

void ObjectObserver::startRenameTracker() {
  std::lock_guard<std::mutex> lock(m_mutex);

  m_check_timer->start();

  const auto top_widgets = getRoots();
  auto objects = std::queue<QObject *>{};
  for (auto top_widget : top_widgets) {
    if (!top_widget->parent()) objects.push(top_widget);
  }

  while (!objects.empty()) {
    auto object = objects.front();
    objects.pop();

    auto object_query = searcher().getQuery(object);
    m_tracked_objects.insert(object, object_query);

    for (const auto child : object->children()) { objects.push(child); }
  }
}

void ObjectObserver::stopRenameTracker() {
  std::lock_guard<std::mutex> lock(m_mutex);

  m_check_timer->stop();
  m_tracked_objects.clear();
}

void ObjectObserver::checkForRenames() {
  std::lock_guard<std::mutex> lock(m_mutex);

  for (auto it = m_tracked_objects.begin(); it != m_tracked_objects.end();
       ++it) {
    const auto &object = it.key();
    auto &query = it.value();

    auto new_query = searcher().getQuery(object);
    if (query != new_query) {
      query = new_query;

      Q_EMIT actionReported(ObservedAction::ObjectRenamed{query});
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
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          m_observed_actions.push_back(recorder_action);
        }

        m_cv.notify_one();
      });
  }
}

bool ObjectObserverQueue::isEmpty() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_observed_actions.empty();
}

ObservedAction ObjectObserverQueue::popAction() {
  std::lock_guard<std::mutex> lock(m_mutex);
  Q_ASSERT(!m_observed_actions.empty());
  return m_observed_actions.takeFirst();
}

ObservedAction ObjectObserverQueue::waitPopAction() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_cv.wait(lock, [this] { return !m_observed_actions.empty(); });

  return m_observed_actions.takeFirst();
}

}// namespace aegis
