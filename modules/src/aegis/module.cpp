/* ----------------------------------- Local -------------------------------- */
#include "aegis/module.h"

#include "aegis/record/strategy.h"
#include "aegis/search/strategy.h"
#include "aegis/service/object.h"
#include "aegis/service/recorder.h"
#include "aegis/service/sniffer.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

std::unique_ptr<AegisModule> AegisModule::m_instance =
    std::unique_ptr<AegisModule>(nullptr);

AegisModule& AegisModule::getInstance() {
  if (!m_instance) m_instance.reset(new AegisModule);

  return *m_instance;
}

void AegisModule::deleteInstance() { m_instance.reset(nullptr); }

AegisModule::AegisModule()
    : m_server(std::make_unique<Server>()),
      m_sniffer(std::make_unique<Sniffer>()),
      m_searcher(std::make_unique<Searcher>()),
      m_recorder(std::make_unique<Recorder>()) {
  m_server->registerService<ObjectService>();
  m_server->registerService<RecorderService>();
  m_server->registerService<SnifferService>();

  m_searcher->addStrategy(std::make_unique<TypeSearch>());
  m_searcher->addStrategy(std::make_unique<PropertiesSearch>());
  m_searcher->addStrategy(std::make_unique<PathSearch>());

  m_recorder->addStrategy(std::make_unique<RecordWidgetStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordButtonStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordComboBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordSpinBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordSliderStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordTabBarStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordToolBoxStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordMenuStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordTextEditStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordLineEditStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordItemViewStrategy>());
  m_recorder->addStrategy(std::make_unique<RecordButtonStrategy>());
}

AegisModule::~AegisModule() = default;

Server& AegisModule::getServer() const { return *m_server; }

Sniffer& AegisModule::getSniffer() const { return *m_sniffer; }

Searcher& AegisModule::getSearcher() const { return *m_searcher; }

Recorder& AegisModule::getRecorder() const { return *m_recorder; }

}  // namespace aegis
