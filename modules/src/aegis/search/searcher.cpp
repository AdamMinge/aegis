/* ----------------------------------- Local -------------------------------- */
#include "aegis/search/searcher.h"

#include "aegis/search/strategy.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QWidget>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------------- Searcher -------------------------- */

Searcher::Searcher() = default;

Searcher::~Searcher() = default;

QObject* Searcher::getObject(const ObjectQuery& query) const {
  const auto objects = findObjects(query, 1);
  return objects.empty() ? nullptr : objects.first();
}

QList<QObject*> Searcher::getObjects(const ObjectQuery& query) const {
  const auto objects = findObjects(query);
  return objects;
}

ObjectQuery Searcher::getQuery(const QObject* object) const {
  auto query = QVariantMap{};
  for (const auto& search_strategy : m_strategies) {
    const auto sub_query = search_strategy->createObjectQuery(object);
    query.insert(sub_query);
  }

  return ObjectQuery(query);
}

void Searcher::addStrategy(std::unique_ptr<SearchStrategy>&& strategy) {
  m_strategies.emplace_back(std::move(strategy));
}

QList<QObject*> Searcher::findObjects(const ObjectQuery& query,
                                      qsizetype limit) const {
  const auto top_widgets = qApp->topLevelWidgets();
  auto objects = std::queue<QObject*>{};
  for (auto top_widget : top_widgets) {
    objects.push(top_widget);
  }

  auto found_objects = QList<QObject*>{};
  while (!objects.empty() && found_objects.size() <= limit) {
    auto parent = objects.front();
    objects.pop();

    const auto matches_query = std::all_of(
        m_strategies.begin(), m_strategies.end(),
        [parent, &query](const auto& search_strategy) {
          return search_strategy->matchesObjectQuery(parent, query.m_data);
        });

    if (matches_query) {
      found_objects.push_back(parent);
    }

    for (const auto child : parent->children()) {
      objects.push(child);
    }
  }

  return found_objects;
}

}  // namespace aegis
