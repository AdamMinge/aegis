/* ----------------------------------- Local -------------------------------- */
#include "aegis/module.h"

#include "aegis/search/strategy.h"
#include "aegis/service/marker.h"
#include "aegis/service/object.h"
#include "aegis/service/recorder.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

std::unique_ptr<AegisModule> AegisModule::m_instance =
  std::unique_ptr<AegisModule>(nullptr);

AegisModule &AegisModule::getInstance() {
  if (!m_instance) m_instance.reset(new AegisModule);

  return *m_instance;
}

void AegisModule::deleteInstance() { m_instance.reset(nullptr); }

AegisModule::AegisModule()
    : m_server(std::make_unique<Server>()),
      m_marker(std::make_unique<Marker>()),
      m_searcher(std::make_unique<Searcher>()) {
  m_server->registerService<RecorderService>();
  m_server->registerService<MarkerService>();
  m_server->registerService<ObjectService>();

  m_searcher->addStrategy(std::make_unique<TypeSearch>());
  m_searcher->addStrategy(std::make_unique<PropertiesSearch>());
  m_searcher->addStrategy(std::make_unique<PathSearch>());
  m_searcher->addStrategy(std::make_unique<OrderIndexSearch>());
  m_searcher->addStrategy(std::make_unique<MemoryAddressSearch>());
}

AegisModule::~AegisModule() = default;

Server &AegisModule::getServer() const { return *m_server; }

Marker &AegisModule::getMarker() const { return *m_marker; }

Searcher &AegisModule::getSearcher() const { return *m_searcher; }

}// namespace aegis
