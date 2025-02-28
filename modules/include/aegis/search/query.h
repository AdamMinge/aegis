#ifndef AEGIS_SEARCH_QUERY_H
#define AEGIS_SEARCH_QUERY_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QVariantMap>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------------- ObjectQuery --------------------------- */

class LIB_AEGIS_API ObjectQuery {
  friend class Searcher;
  friend class CommonQueries;

public:
  [[nodiscard]] static ObjectQuery fromString(const QString &id);

public:
  explicit ObjectQuery();
  ~ObjectQuery();

  [[nodiscard]] QString toString() const;
  [[nodiscard]] bool isValid() const;

protected:
  explicit ObjectQuery(QVariantMap data);

private:
  QVariantMap m_data;
};

/* --------------------------------- CommonQueries -------------------------- */

class LIB_AEGIS_API CommonQueries {
public:
  [[nodiscard]] static ObjectQuery createPathObjectQuery(const QString &path);
  [[nodiscard]] static ObjectQuery createTypeObjectQuery(const QMetaType &type);
  [[nodiscard]] static ObjectQuery
  createPropertiesObjectQuery(const QVariantMap &properties);
};

}// namespace aegis

#endif// AEGIS_SEARCH_QUERY_H