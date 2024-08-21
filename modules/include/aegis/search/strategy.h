#ifndef AEGIS_SEARCH_STRATEGY_H
#define AEGIS_SEARCH_STRATEGY_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QLatin1String>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SearchStrategy ---------------------------- */

class LIB_AEGIS_API SearchStrategy {
 public:
  explicit SearchStrategy();
  virtual ~SearchStrategy();

  [[nodiscard]] virtual bool matchesObjectQuery(
      QObject* object, const QVariantMap& query) const = 0;
  [[nodiscard]] virtual QVariantMap createObjectQuery(
      QObject* object) const = 0;
};

/* -------------------------------- TypeSearch ------------------------------ */

class LIB_AEGIS_API TypeSearch : public SearchStrategy {
 public:
  static constexpr QLatin1String type_query = QLatin1String("type");

 public:
  explicit TypeSearch();
  ~TypeSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
      QObject* object, const QVariantMap& query) const override;
  [[nodiscard]] QVariantMap createObjectQuery(QObject* object) const override;
};

/* ----------------------------- PropertiesSearch --------------------------- */

class LIB_AEGIS_API PropertiesSearch : public SearchStrategy {
 public:
  static constexpr QLatin1String properties_query = QLatin1String("properties");

 public:
  explicit PropertiesSearch();
  ~PropertiesSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
      QObject* object, const QVariantMap& query) const override;
  [[nodiscard]] QVariantMap createObjectQuery(QObject* object) const override;

 private:
  [[nodiscard]] static QSet<QString> getUsedProperties(QObject* object);
  [[nodiscard]] static QMap<int, QSet<QString>> getTypeToProperties();
};

/* -------------------------------- PathSearch ------------------------------ */

class LIB_AEGIS_API PathSearch : public SearchStrategy {
 public:
  static constexpr QLatin1String path_query = QLatin1String("path");

 public:
  explicit PathSearch();
  ~PathSearch() override;

  [[nodiscard]] bool matchesObjectQuery(
      QObject* object, const QVariantMap& query) const override;
  [[nodiscard]] QVariantMap createObjectQuery(QObject* object) const override;

 private:
  [[nodiscard]] QString getPath(QObject* object) const;
};

}  // namespace aegis

#endif  // AEGIS_SEARCH_STRATEGY_H