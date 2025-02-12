/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/searcher.h"

#include "aegis/module.h"
#include "aegis/search/utils.h"
#include "aegis/service/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QWidget>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------ SearcherTreeCall -------------------------- */

SearcherTreeCall::SearcherTreeCall(
  aegis_proto::Searcher::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Searcher::AsyncService::RequestTree) {}

SearcherTreeCall::~SearcherTreeCall() = default;

std::unique_ptr<SearcherTreeCallData> SearcherTreeCall::clone() const {
  return std::make_unique<SearcherTreeCall>(getService(), getQueue());
}

SearcherTreeCall::ProcessResult
SearcherTreeCall::process(const Request &request) const {
  const auto opt_query = std::optional<ObjectQuery>{};
  auto objects = QObjectList{};

  if (request.has_object()) {
    auto [status, object] = tryGetSingleObject(*opt_query);
    if (!status.ok()) return {status, {}};
    objects.append(object);
  } else {
    objects.append(getTopLevelObjects());
  }

  const auto response = tree(objects);
  return {grpc::Status::OK, response};
}

SearcherTreeCall::Response
SearcherTreeCall::tree(const QObjectList &objects) const {
  auto response = SearcherTreeCall::Response{};

  auto objectsToProcess =
    std::queue<std::pair<QObject *, aegis_proto::TreeNode *>>{};
  for (const auto object : objects) {
    objectsToProcess.push(std::make_pair(object, response.add_nodes()));
  }

  while (!objectsToProcess.empty()) {
    auto objectToProcess = objectsToProcess.front();
    auto object = objectToProcess.first;
    auto object_children = objectToProcess.second;
    objectsToProcess.pop();

    const auto object_query = searcher().getQuery(object);
    object_children->set_object(object_query.toString().toStdString());

    for (const auto child : object->children()) {
      objectsToProcess.push(
        std::make_pair(child, object_children->add_nodes()));
    }
  }


  return response;
}

/* --------------------------- SearcherListenChangesCall -------------------- */

SearcherListenChangesCall::SearcherListenChangesCall(
  aegis_proto::Searcher::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : StreamCallData(
        service, queue, CallTag{this},
        &aegis_proto::Searcher::AsyncService::RequestListenTreeChanges) {}

SearcherListenChangesCall::~SearcherListenChangesCall() = default;

SearcherListenChangesCall::ProcessResult
SearcherListenChangesCall::process(const Request &request) const {
  return {};
}

std::unique_ptr<SearcherListenChangesCallData>
SearcherListenChangesCall::clone() const {
  return std::make_unique<SearcherListenChangesCall>(getService(), getQueue());
}

/* ------------------------------ SearcherFindCall -------------------------- */

SearcherFindCall::SearcherFindCall(
  aegis_proto::Searcher::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Searcher::AsyncService::RequestFind) {}

SearcherFindCall::~SearcherFindCall() = default;

std::unique_ptr<SearcherFindCallData> SearcherFindCall::clone() const {
  return std::make_unique<SearcherFindCall>(getService(), getQueue());
}

SearcherFindCall::ProcessResult
SearcherFindCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, objects] = tryGetObjects(query);
  if (!status.ok()) return {status, {}};

  const auto response = find(objects);
  return {grpc::Status::OK, response};
}

SearcherFindCall::Response
SearcherFindCall::find(const QObjectList &objects) const {
  auto response = SearcherFindCall::Response{};
  for (const auto object : objects) {
    response.add_objects(searcher().getQuery(object).toString().toStdString());
  }

  return response;
}

/* ------------------------------ SearcherService --------------------------- */

SearcherService::SearcherService() = default;

SearcherService::~SearcherService() = default;

void SearcherService::start(grpc::ServerCompletionQueue *queue) {
  auto tree_call = new SearcherTreeCall(this, queue);
  auto tree_listen_changes = new SearcherListenChangesCall(this, queue);
  auto find_call = new SearcherFindCall(this, queue);

  tree_call->proceed();
  tree_listen_changes->proceed();
  find_call->proceed();
}

}// namespace aegis
