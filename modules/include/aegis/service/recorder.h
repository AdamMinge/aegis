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

/* ------------------------------ RecorderStartCall ------------------------- */

using RecorderStartCallData =
    CallData<aegis_proto::Recorder::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API RecorderStartCall : public RecorderStartCallData {
 public:
  explicit RecorderStartCall(aegis_proto::Recorder::AsyncService* service,
                             grpc::ServerCompletionQueue* queue);
  ~RecorderStartCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<RecorderStartCallData> clone() const override;
};

/* ------------------------------ RecorderStopCall -------------------------- */

using RecorderStopCallData =
    CallData<aegis_proto::Recorder::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API RecorderStopCall : public RecorderStopCallData {
 public:
  explicit RecorderStopCall(aegis_proto::Recorder::AsyncService* service,
                            grpc::ServerCompletionQueue* queue);
  ~RecorderStopCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<RecorderStopCallData> clone() const override;
};

/* ------------------------------ RecorderClearCall ------------------------- */

using RecorderClearCallData =
    CallData<aegis_proto::Recorder::AsyncService, google::protobuf::Empty,
             google::protobuf::Empty>;

class LIB_AEGIS_API RecorderClearCall : public RecorderClearCallData {
 public:
  explicit RecorderClearCall(aegis_proto::Recorder::AsyncService* service,
                             grpc::ServerCompletionQueue* queue);
  ~RecorderClearCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<RecorderClearCallData> clone() const override;
};

/* ------------------------------ RecorderReportCall ------------------------ */

using RecorderReportCallData =
    CallData<aegis_proto::Recorder::AsyncService, google::protobuf::Empty,
             aegis_proto::ReportResponse>;

class LIB_AEGIS_API RecorderReportCall : public RecorderReportCallData {
 public:
  explicit RecorderReportCall(aegis_proto::Recorder::AsyncService* service,
                              grpc::ServerCompletionQueue* queue);
  ~RecorderReportCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<RecorderReportCallData> clone() const override;
};

/* ------------------------------ RecorderService --------------------------- */

class RecorderService
    : public ServiceWrapper<aegis_proto::Recorder::AsyncService> {
 public:
  explicit RecorderService();
  ~RecorderService() override;

  void start(grpc::ServerCompletionQueue* queue) override;
};

}  // namespace aegis

#endif  // AEGIS_SERVICE_RECORDER_H
