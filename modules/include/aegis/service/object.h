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

/* -------------------------------- ObjectTreeCall -------------------------- */

using ObjectTreeCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::OptionalObjectRequest,
  aegis_proto::TreeResponse>;

class LIB_AEGIS_API ObjectTreeCall : public ObjectTreeCallData {
public:
  explicit ObjectTreeCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectTreeCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectTreeCallData> clone() const override;

private:
  [[nodiscard]] Response tree(const QObjectList &objects) const;
};

/* -------------------------------- ObjectFindCall -------------------------- */

using ObjectFindCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::FindRequest,
  aegis_proto::FindResponse>;

class LIB_AEGIS_API ObjectFindCall : public ObjectFindCallData {
public:
  explicit ObjectFindCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectFindCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectFindCallData> clone() const override;

private:
  [[nodiscard]] Response find(const QObjectList &objects) const;
};

/* ------------------------------ ObjectParentCall ------------------------ */

using ObjectParentCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
  aegis_proto::ParentResponse>;

class LIB_AEGIS_API ObjectParentCall : public ObjectParentCallData {
public:
  explicit ObjectParentCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectParentCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectParentCallData> clone() const override;

private:
  [[nodiscard]] Response parent(const QObject *object) const;
};

/* ----------------------------- ObjectChildrenCall ----------------------- */

using ObjectChildrenCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
  aegis_proto::ChildrenResponse>;

class LIB_AEGIS_API ObjectChildrenCall : public ObjectChildrenCallData {
public:
  explicit ObjectChildrenCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectChildrenCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectChildrenCallData> clone() const override;

private:
  [[nodiscard]] Response children(const QObject *object) const;
};

/* --------------------------- ObjectInvokeMethodCall --------------------- */

using ObjectInvokeMethodCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::InvokeMethodRequest,
  google::protobuf::Empty>;

class LIB_AEGIS_API ObjectInvokeMethodCall : public ObjectInvokeMethodCallData {
public:
  explicit ObjectInvokeMethodCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectInvokeMethodCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectInvokeMethodCallData> clone() const override;

private:
  [[nodiscard]] ProcessResult invoke(
    QObject *object, const std::string &method,
    const google::protobuf::RepeatedPtrField<google::protobuf::Value>
      &arguments) const;
  [[nodiscard]] QMetaMethod
  metaMethod(const QObject *object, const std::string &name) const;
};

/* ---------------------------- ObjectSetPropertyCall --------------------- */

using ObjectSetPropertyCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::SetPropertyRequest,
  google::protobuf::Empty>;

class LIB_AEGIS_API ObjectSetPropertyCall : public ObjectSetPropertyCallData {
public:
  explicit ObjectSetPropertyCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectSetPropertyCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectSetPropertyCallData> clone() const override;

private:
  [[nodiscard]] ProcessResult setProperty(
    QObject *object, const std::string &property,
    const google::protobuf::Value &value) const;
};

/* ---------------------------- ObjectDumpMethodsCall --------------------- */

using ObjectDumpMethodsCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
  aegis_proto::DumpMethodsResponse>;

class LIB_AEGIS_API ObjectDumpMethodsCall : public ObjectDumpMethodsCallData {
public:
  explicit ObjectDumpMethodsCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectDumpMethodsCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectDumpMethodsCallData> clone() const override;

private:
  [[nodiscard]] aegis_proto::DumpMethodsResponse
  methods(const QObject *object) const;
};

/* --------------------------- ObjectDumpPropertiesCall ------------------- */

using ObjectDumpPropertiesCallData = CallData<
  aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
  aegis_proto::DumpPropertiesResponse>;

class LIB_AEGIS_API ObjectDumpPropertiesCall
    : public ObjectDumpPropertiesCallData {
public:
  explicit ObjectDumpPropertiesCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectDumpPropertiesCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectDumpPropertiesCallData> clone() const override;

private:
  [[nodiscard]] aegis_proto::DumpPropertiesResponse
  properties(const QObject *object) const;
};

/* ---------------------------- ObjectListenChangesCall --------------------- */

using ObjectListenChangesCallData = StreamCallData<
  aegis_proto::Object::AsyncService, google::protobuf::Empty,
  aegis_proto::ObjectChangeResponse>;

class LIB_AEGIS_API ObjectListenChangesCall
    : public ObjectListenChangesCallData {
public:
  explicit ObjectListenChangesCall(
    aegis_proto::Object::AsyncService *service,
    grpc::ServerCompletionQueue *queue);
  ~ObjectListenChangesCall() override;

  ProcessResult process(const Request &request) const override;

  std::unique_ptr<ObjectListenChangesCallData> clone() const override;

private:
  std::unique_ptr<ObjectObserver> m_observer;
  std::unique_ptr<ObjectObserverQueue> m_observer_queue;
  std::unique_ptr<ObservedActionsMapper> m_mapper;
};

/* ------------------------------- ObjectService -------------------------- */

class ObjectService : public ServiceWrapper<aegis_proto::Object::AsyncService> {
public:
  explicit ObjectService();
  ~ObjectService() override;

  void start(grpc::ServerCompletionQueue *queue) override;
};

}// namespace aegis

#endif// AEGIS_SERVICE_OBJECT_H
