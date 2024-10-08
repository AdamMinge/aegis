#ifndef AEGIS_MODULE_H
#define AEGIS_MODULE_H

/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/mark/marker.h"
#include "aegis/record/recorder.h"
#include "aegis/search/searcher.h"
#include "aegis/server/server.h"
#include "aegis/sniff/sniffer.h"
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
  [[nodiscard]] Sniffer& getSniffer() const;
  [[nodiscard]] Searcher& getSearcher() const;
  [[nodiscard]] Recorder& getRecorder() const;

 protected:
  explicit AegisModule();

 private:
  static std::unique_ptr<AegisModule> m_instance;

  std::unique_ptr<Server> m_server;
  std::unique_ptr<Marker> m_marker;
  std::unique_ptr<Sniffer> m_sniffer;
  std::unique_ptr<Searcher> m_searcher;
  std::unique_ptr<Recorder> m_recorder;
};

inline Server& server() { return AegisModule::getInstance().getServer(); }

inline Marker& marker() { return AegisModule::getInstance().getMarker(); }

inline Sniffer& sniffer() { return AegisModule::getInstance().getSniffer(); }

inline Searcher& searcher() { return AegisModule::getInstance().getSearcher(); }

inline Recorder& recorder() { return AegisModule::getInstance().getRecorder(); }

}  // namespace aegis

#endif  // AEGIS_MODULE_H