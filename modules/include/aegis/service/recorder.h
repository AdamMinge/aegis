#ifndef AEGIS_SERVICE_RECORDER_H
#define AEGIS_SERVICE_RECORDER_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/server/call.h"
#include "aegis/server/service.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ActionRecorder;
class ActionRecorderQueue;
class RecordedActionsMapper;

/* ------------------------- RecorderListenCommandsCall -------------------- */

using RecorderListenCommandsCallData = StreamCallData<
  aegis_proto::RecorderService::AsyncService, google::protobuf::Empty,
  aegis_proto::RecorderCommand>;

class LIB_AEGIS_API RecorderListenCommandsCall
    : public RecorderListenCommandsCallData {
public:
  explicit RecorderListenCommandsCall(
    aegis_proto::RecorderService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~RecorderListenCommandsCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<RecorderListenCommandsCallData> clone() const override;

private:
  std::unique_ptr<ActionRecorder> m_recorder;
  std::unique_ptr<ActionRecorderQueue> m_recorder_queue;
  std::unique_ptr<RecordedActionsMapper> m_mapper;
};

/* ------------------------------ RecorderService --------------------------- */

class RecorderService
    : public ServiceWrapper<aegis_proto::RecorderService::AsyncService> {
public:
  explicit RecorderService();
  ~RecorderService() override;

  void start(grpc::ServerCompletionQueue *queue) override;
};

}// namespace aegis

#endif// AEGIS_SERVICE_RECORDER_H
