#ifndef AEGIS_SEARCH_QUERY_H
#define AEGIS_SEARCH_QUERY_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
/* --------------------------------- Standard ------------------------------- */
#include <list>
#include <map>
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/search/query.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class SearchStrategy;

/* ----------------------------------- Searcher ----------------------------- */

class LIB_AEGIS_API Searcher : public QObject {
  Q_OBJECT

 public:
  explicit Searcher(QObject* parent = nullptr);
  ~Searcher() override;

  [[nodiscard]] QObject* getObject(const ObjectQuery& query) const;
  [[nodiscard]] QList<QObject*> getObjects(const ObjectQuery& query) const;

  [[nodiscard]] ObjectQuery getQuery(QObject* object) const;

 private:
  [[nodiscard]] QList<QObject*> findObjects(
      const ObjectQuery& query,
      qsizetype limit = std::numeric_limits<qsizetype>::max()) const;

 private:
  std::list<std::unique_ptr<SearchStrategy>> m_search_strategies;
};

}  // namespace aegis

#endif  // AEGIS_SEARCH_QUERY_H