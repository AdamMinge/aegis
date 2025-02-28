#ifndef AEGIS_SERVICE_OBJECT_H
#define AEGIS_SERVICE_OBJECT_H

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
class ObjectObserver;
class ObjectObserverQueue;
class ObservedActionsMapper;

/* ------------------------------- ObjectGetTreeCall ------------------------ */

using ObjectGetTreeCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::OptionalObject,
  aegis_proto::ObjectTree>;

class LIB_AEGIS_API ObjectGetTreeCall : public ObjectGetTreeCallData {
public:
  explicit ObjectGetTreeCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectGetTreeCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectGetTreeCallData> clone() const override;

private:
  [[nodiscard]] Response tree(const QObjectList &objects) const;
};

/* -------------------------------- ObjectFindCall -------------------------- */

using ObjectFindCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::Object,
  aegis_proto::Objects>;

class LIB_AEGIS_API ObjectFindCall : public ObjectFindCallData {
public:
  explicit ObjectFindCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectFindCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectFindCallData> clone() const override;

private:
  [[nodiscard]] Response find(const QObjectList &objects) const;
};

/* ------------------------------ ObjectParentCall ------------------------ */

using ObjectParentCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::Object,
  aegis_proto::Object>;

class LIB_AEGIS_API ObjectParentCall : public ObjectParentCallData {
public:
  explicit ObjectParentCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectParentCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectParentCallData> clone() const override;

private:
  [[nodiscard]] Response parent(const QObject *object) const;
};

/* ----------------------------- ObjectChildrenCall ----------------------- */

using ObjectChildrenCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::Object,
  aegis_proto::Objects>;

class LIB_AEGIS_API ObjectChildrenCall : public ObjectChildrenCallData {
public:
  explicit ObjectChildrenCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectChildrenCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectChildrenCallData> clone() const override;

private:
  [[nodiscard]] Response children(const QObject *object) const;
};

/* ---------------------------- ObjectCallMethodCall ---------------------- */

using ObjectCallMethodCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::MethodCall,
  google::protobuf::Empty>;

class LIB_AEGIS_API ObjectCallMethodCall : public ObjectCallMethodCallData {
public:
  explicit ObjectCallMethodCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectCallMethodCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectCallMethodCallData> clone() const override;

private:
  [[nodiscard]] ProcessResult invoke(
    QObject *object, const std::string &method,
    const google::protobuf::RepeatedPtrField<google::protobuf::Value>
      &arguments) const;
  [[nodiscard]] QMetaMethod
  metaMethod(const QObject *object, const std::string &name) const;
};

/* -------------------------- ObjectUpdatePropertyCall -------------------- */

using ObjectUpdatePropertyCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::PropertyUpdate,
  google::protobuf::Empty>;

class LIB_AEGIS_API ObjectUpdatePropertyCall
    : public ObjectUpdatePropertyCallData {
public:
  explicit ObjectUpdatePropertyCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectUpdatePropertyCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectUpdatePropertyCallData> clone() const override;

private:
  [[nodiscard]] ProcessResult setProperty(
    QObject *object, const std::string &property,
    const google::protobuf::Value &value) const;
};

/* ---------------------------- ObjectGetMethodsCall ---------------------- */

using ObjectGetMethodsCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::Object,
  aegis_proto::Methods>;

class LIB_AEGIS_API ObjectGetMethodsCall : public ObjectGetMethodsCallData {
public:
  explicit ObjectGetMethodsCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectGetMethodsCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectGetMethodsCallData> clone() const override;

private:
  [[nodiscard]] Response methods(const QObject *object) const;
};

/* --------------------------- ObjectGetPropertiesCall -------------------- */

using ObjectGetPropertiesCallData = CallData<
  aegis_proto::ObjectService::AsyncService, aegis_proto::Object,
  aegis_proto::Properties>;

class LIB_AEGIS_API ObjectGetPropertiesCall
    : public ObjectGetPropertiesCallData {
public:
  explicit ObjectGetPropertiesCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectGetPropertiesCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectGetPropertiesCallData> clone() const override;

private:
  [[nodiscard]] Response properties(const QObject *object) const;
};

/* ------------------------- ObjectListenObjectChangesCall ------------------ */

using ObjectListenObjectChangesCallData = StreamCallData<
  aegis_proto::ObjectService::AsyncService, google::protobuf::Empty,
  aegis_proto::ObjectChange>;

class LIB_AEGIS_API ObjectListenObjectChangesCall
    : public ObjectListenObjectChangesCallData {
public:
  explicit ObjectListenObjectChangesCall(
    aegis_proto::ObjectService::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectListenObjectChangesCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectListenObjectChangesCallData> clone() const override;

private:
  std::unique_ptr<ObjectObserver> m_observer;
  std::unique_ptr<ObjectObserverQueue> m_observer_queue;
  std::unique_ptr<ObservedActionsMapper> m_mapper;
};

/* ------------------------------- ObjectService -------------------------- */

class ObjectService
    : public ServiceWrapper<aegis_proto::ObjectService::AsyncService> {
public:
  explicit ObjectService();
  ~ObjectService() override;

  void start(grpc::ServerCompletionQueue *queue) override;
};

}// namespace aegis

#endif// AEGIS_SERVICE_OBJECT_H
