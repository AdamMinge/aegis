/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/search/searcher.h"
#include "aegis/search/strategy.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------------- ObjectQuery --------------------------- */

ObjectQuery::ObjectQuery() : ObjectQuery(QVariantMap{}) {}

ObjectQuery::ObjectQuery(QVariantMap data) : m_data(std::move(data)) {}

ObjectQuery::~ObjectQuery() = default;

ObjectQuery ObjectQuery::fromString(const QString& id) {
  auto data = QVariantMap{};

  auto parser_error = QJsonParseError{};
  const auto json_document =
      QJsonDocument::fromJson(id.toUtf8(), &parser_error);

  if (parser_error.error == QJsonParseError::NoError) {
    const auto json_object = json_document.object();

    for (auto it = json_object.begin(); it != json_object.end(); ++it) {
      data.insert(it.key(), it.value().toVariant());
    }
  }

  return ObjectQuery(data);
}

QString ObjectQuery::toString() const {
  auto json_object = QJsonObject{};
  for (auto it = m_data.begin(); it != m_data.end(); ++it) {
    json_object.insert(it.key(), QJsonValue::fromVariant(it.value()));
  }

  const auto json_document = QJsonDocument(json_object);
  return QString(json_document.toJson(QJsonDocument::Compact));
}

bool ObjectQuery::isValid() const { return !m_data.isEmpty(); }

/* --------------------------------- CommonQueries -------------------------- */

ObjectQuery CommonQueries::createPathObjectQuery(const QString& path) {
  return ObjectQuery(
      QVariantMap{{PathSearch::path_query, QVariant::fromValue(path)}});
}

ObjectQuery CommonQueries::createTypeObjectQuery(const QMetaType& type) {
  return ObjectQuery(
      QVariantMap{{TypeSearch::type_query, QVariant::fromValue(type.name())}});
}

ObjectQuery CommonQueries::createPropertiesObjectQuery(
    const QVariantMap& properties) {
  return ObjectQuery(QVariantMap{
      {PropertiesSearch::properties_query, QVariant::fromValue(properties)}});
}

/* ----------------------------------- Searcher ----------------------------- */

Searcher::Searcher(QObject* parent) : QObject(parent) {
  m_search_strategies.emplace_back(std::make_unique<TypeSearch>());
  m_search_strategies.emplace_back(std::make_unique<PropertiesSearch>());
  m_search_strategies.emplace_back(std::make_unique<PathSearch>());
}

Searcher::~Searcher() = default;

QObject* Searcher::getObject(const ObjectQuery& query) const {
  const auto objects = findObjects(query, 1);
  return objects.empty() ? nullptr : objects.first();
}

QList<QObject*> Searcher::getObjects(const ObjectQuery& query) const {
  const auto objects = findObjects(query);
  return objects;
}

ObjectQuery Searcher::getId(QObject* object) const {
  auto query = QVariantMap{};
  for (const auto& search_strategy : m_search_strategies) {
    const auto sub_query = search_strategy->createObjectQuery(object);
    query.insert(sub_query);
  }

  return ObjectQuery(query);
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
        m_search_strategies.begin(), m_search_strategies.end(),
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
