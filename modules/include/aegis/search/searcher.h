#ifndef AEGIS_SEARCH_SEARCHER_H
#define AEGIS_SEARCH_SEARCHER_H

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
  explicit Searcher();
  ~Searcher() override;

  [[nodiscard]] QObject *getObject(const ObjectQuery &query) const;
  [[nodiscard]] QList<QObject *> getObjects(const ObjectQuery &query) const;

  [[nodiscard]] ObjectQuery getQuery(const QObject *object) const;

  void addStrategy(std::unique_ptr<SearchStrategy> &&strategy);

Q_SIGNALS:
  void objectRemoved(const ObjectQuery &parent, const ObjectQuery &object);
  void objectAdded(const ObjectQuery &parent, const ObjectQuery &object);
  void objectReplaced(const ObjectQuery &from, const ObjectQuery &to);

private:
  [[nodiscard]] QList<QObject *> findObjects(
    const ObjectQuery &query,
    qsizetype limit = std::numeric_limits<qsizetype>::max()) const;

private:
  std::list<std::unique_ptr<SearchStrategy>> m_strategies;
};

}// namespace aegis

#endif// AEGIS_SEARCH_SEARCHER_H