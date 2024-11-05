#ifndef AEGIS_MODULE_H
#define AEGIS_MODULE_H

/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/mark/marker.h"
#include "aegis/search/searcher.h"
#include "aegis/server/server.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- AegisModule ----------------------------- */

class LIB_AEGIS_API AegisModule {
 public:
  [[nodiscard]] static AegisModule& getInstance();
  static void deleteInstance();

 public:
  ~AegisModule();

  [[nodiscard]] Server& getServer() const;
  [[nodiscard]] Marker& getMarker() const;
  [[nodiscard]] Searcher& getSearcher() const;

 protected:
  explicit AegisModule();

 private:
  static std::unique_ptr<AegisModule> m_instance;

  std::unique_ptr<Server> m_server;
  std::unique_ptr<Marker> m_marker;
  std::unique_ptr<Searcher> m_searcher;
};

inline Server& server() { return AegisModule::getInstance().getServer(); }

inline Marker& marker() { return AegisModule::getInstance().getMarker(); }

inline Searcher& searcher() { return AegisModule::getInstance().getSearcher(); }

}  // namespace aegis

#endif  // AEGIS_MODULE_H