#ifndef AEGIS_SEARCH_STRATEGY_H
#define AEGIS_SEARCH_STRATEGY_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QLatin1String>
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SearchStrategy ---------------------------- */

class LIB_AEGIS_API SearchStrategy {
public:
  explicit SearchStrategy();
  virtual ~SearchStrategy();

  [[nodiscard]] virtual bool
  matchesObjectQuery(const QObject *object, const QVariantMap &query) const = 0;
  [[nodiscard]] virtual QVariantMap
  createObjectQuery(const QObject *object) const = 0;
};

/* -------------------------------- TypeSearch ------------------------------ */

class LIB_AEGIS_API TypeSearch : public SearchStrategy {
public:
  static constexpr auto type_query = "type";

public:
  explicit TypeSearch();
  ~TypeSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
    const QObject *object, const QVariantMap &query) const override;
  [[nodiscard]] QVariantMap
  createObjectQuery(const QObject *object) const override;
};

/* ----------------------------- PropertiesSearch --------------------------- */

class LIB_AEGIS_API PropertiesSearch : public SearchStrategy {
public:
  static constexpr auto properties_query = "properties";

public:
  explicit PropertiesSearch();
  ~PropertiesSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
    const QObject *object, const QVariantMap &query) const override;
  [[nodiscard]] QVariantMap
  createObjectQuery(const QObject *object) const override;

private:
  [[nodiscard]] static QSet<QString> getUsedProperties(const QObject *object);
  [[nodiscard]] static QMap<int, QSet<QString>> getTypeToProperties();
};

/* -------------------------------- PathSearch ------------------------------ */

class LIB_AEGIS_API PathSearch : public SearchStrategy {
public:
  static constexpr auto path_query = "path";

public:
  explicit PathSearch();
  ~PathSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
    const QObject *object, const QVariantMap &query) const override;
  [[nodiscard]] QVariantMap
  createObjectQuery(const QObject *object) const override;

private:
  [[nodiscard]] QString getPath(const QObject *object) const;
};

/* -------------------------------- OrderIndex ------------------------------ */

class LIB_AEGIS_API OrderIndex : public SearchStrategy {
public:
  static constexpr auto order_index_query = "order_index";

public:
  explicit OrderIndex();
  ~OrderIndex() override;

  [[nodiscard]] bool matchesObjectQuery(
    const QObject *object, const QVariantMap &query) const override;
  [[nodiscard]] QVariantMap
  createObjectQuery(const QObject *object) const override;

private:
  [[nodiscard]] uint getOrderIndex(const QObject *object) const;
};

}// namespace aegis

#endif// AEGIS_SEARCH_STRATEGY_H