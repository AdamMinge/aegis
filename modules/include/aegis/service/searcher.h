#ifndef AEGIS_SERVICE_SEARCHER_H
#define AEGIS_SERVICE_SEARCHER_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ------------------------------------ Qt ---------------------------------- */
#include <QMetaMethod>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/server/call.h"
#include "aegis/server/service.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ------------------------------- SearcherTreeCall ------------------------- */

using SearcherTreeCallData = CallData<
  aegis_proto::Searcher::AsyncService, aegis_proto::OptionalObjectRequest,
  aegis_proto::TreeResponse>;

class LIB_AEGIS_API SearcherTreeCall : public SearcherTreeCallData {
public:
  explicit SearcherTreeCall(
    aegis_proto::Searcher::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~SearcherTreeCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<SearcherTreeCallData> clone() const override;

private:
  [[nodiscard]] Response tree(const QObjectList &objects) const;
};


/* --------------------------- SearcherListenChangesCall -------------------- */

using SearcherListenChangesCallData = StreamCallData<
  aegis_proto::Searcher::AsyncService, google::protobuf::Empty,
  aegis_proto::TreeChangeResponse>;

class LIB_AEGIS_API SearcherListenChangesCall
    : public SearcherListenChangesCallData {
public:
  explicit SearcherListenChangesCall(
    aegis_proto::Searcher::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~SearcherListenChangesCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<SearcherListenChangesCallData> clone() const override;
};

/* ------------------------------- SearcherFindCall ------------------------- */

using SearcherFindCallData = CallData<
  aegis_proto::Searcher::AsyncService, aegis_proto::FindRequest,
  aegis_proto::FindResponse>;

class LIB_AEGIS_API SearcherFindCall : public SearcherFindCallData {
public:
  explicit SearcherFindCall(
    aegis_proto::Searcher::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~SearcherFindCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<SearcherFindCallData> clone() const override;

private:
  [[nodiscard]] Response find(const QObjectList &objects) const;
};

/* ------------------------------- SearcherService -------------------------- */

class SearcherService
    : public ServiceWrapper<aegis_proto::Searcher::AsyncService> {
public:
  explicit SearcherService();
  ~SearcherService() override;

  void start(grpc::ServerCompletionQueue *queue) override;
};

}// namespace aegis

#endif// AEGIS_SERVICE_SEARCHER_H
