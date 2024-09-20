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
/* -------------------------------------------------------------------------- */

namespace aegis {

class ObjectQuery;

/* ------------------------------ ObjectFindCallData ------------------------ */

using ObjectFindCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
             aegis_proto::FindResponse>;

class LIB_AEGIS_API ObjectFindCall : public ObjectFindCallData {
 public:
  explicit ObjectFindCall(aegis_proto::Object::AsyncService* service,
                          grpc::ServerCompletionQueue* queue);
  ~ObjectFindCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectFindCallData> clone() const override;

 private:
  [[nodiscard]] Response find(const ObjectQuery& query) const;
};

/* ----------------------------- ObjectParentCallData ----------------------- */

using ObjectParentCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
             aegis_proto::ParentResponse>;

class LIB_AEGIS_API ObjectParentCall : public ObjectParentCallData {
 public:
  explicit ObjectParentCall(aegis_proto::Object::AsyncService* service,
                            grpc::ServerCompletionQueue* queue);
  ~ObjectParentCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectParentCallData> clone() const override;

 private:
  [[nodiscard]] Response find(const ObjectQuery& query) const;
  [[nodiscard]] ObjectQuery parent(const QObject* object) const;
};

/* ---------------------------- ObjectChildrenCallData ---------------------- */

using ObjectChildrenCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
             aegis_proto::ChildrenResponse>;

class LIB_AEGIS_API ObjectChildrenCall : public ObjectChildrenCallData {
 public:
  explicit ObjectChildrenCall(aegis_proto::Object::AsyncService* service,
                              grpc::ServerCompletionQueue* queue);
  ~ObjectChildrenCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectChildrenCallData> clone() const override;

 private:
  [[nodiscard]] Response find(const ObjectQuery& query) const;
  [[nodiscard]] QList<ObjectQuery> children(const QObject* object) const;
};

/* -------------------------- ObjectInvokeMethodCallData -------------------- */

using ObjectInvokeMethodCallData =
    CallData<aegis_proto::Object::AsyncService,
             aegis_proto::InvokeMethodRequest, google::protobuf::Empty>;

class LIB_AEGIS_API ObjectInvokeMethodCall : public ObjectInvokeMethodCallData {
 public:
  explicit ObjectInvokeMethodCall(aegis_proto::Object::AsyncService* service,
                                  grpc::ServerCompletionQueue* queue);
  ~ObjectInvokeMethodCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectInvokeMethodCallData> clone() const override;

 private:
  [[nodiscard]] ProcessResult invoke(
      const ObjectQuery& query, const std::string& method,
      const google::protobuf::RepeatedPtrField<google::protobuf::Value>&
          arguments) const;
  [[nodiscard]] QMetaMethod metaMethod(const QObject* object,
                                       const std::string& name) const;
};

/* --------------------------- ObjectSetPropertyCallData -------------------- */

using ObjectSetPropertyCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::SetPropertyRequest,
             google::protobuf::Empty>;

class LIB_AEGIS_API ObjectSetPropertyCall : public ObjectSetPropertyCallData {
 public:
  explicit ObjectSetPropertyCall(aegis_proto::Object::AsyncService* service,
                                 grpc::ServerCompletionQueue* queue);
  ~ObjectSetPropertyCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectSetPropertyCallData> clone() const override;

 private:
  [[nodiscard]] ProcessResult setProperty(
      const ObjectQuery& query, const std::string& property,
      const google::protobuf::Value& value) const;
};

/* --------------------------- ObjectDumpMethodsCallData -------------------- */

using ObjectDumpMethodsCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
             aegis_proto::DumpMethodsResponse>;

class LIB_AEGIS_API ObjectDumpMethodsCall : public ObjectDumpMethodsCallData {
 public:
  explicit ObjectDumpMethodsCall(aegis_proto::Object::AsyncService* service,
                                 grpc::ServerCompletionQueue* queue);
  ~ObjectDumpMethodsCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectDumpMethodsCallData> clone() const override;

 private:
  [[nodiscard]] ProcessResult dump(const ObjectQuery& query) const;
  [[nodiscard]] aegis_proto::ObjectMethods methods(const QObject* object) const;
};

/* -------------------------- ObjectDumpPropertiesCallData ------------------ */

using ObjectDumpPropertiesCallData =
    CallData<aegis_proto::Object::AsyncService, aegis_proto::ObjectRequest,
             aegis_proto::DumpPropertiesResponse>;

class LIB_AEGIS_API ObjectDumpPropertiesCall
    : public ObjectDumpPropertiesCallData {
 public:
  explicit ObjectDumpPropertiesCall(aegis_proto::Object::AsyncService* service,
                                    grpc::ServerCompletionQueue* queue);
  ~ObjectDumpPropertiesCall() override;

  ProcessResult process(const Request& request) const override;

  std::unique_ptr<ObjectDumpPropertiesCallData> clone() const override;

 private:
  [[nodiscard]] ProcessResult dump(const ObjectQuery& query) const;
  [[nodiscard]] aegis_proto::ObjectProperties properties(
      const QObject* object) const;
};

/* -------------------------------- ObjectService --------------------------- */

class LIB_AEGIS_API ObjectService : public aegis_proto::Object::AsyncService {
 public:
  explicit ObjectService(grpc::ServerCompletionQueue* queue);
  ~ObjectService() override;
};

}  // namespace aegis

#endif  // AEGIS_SERVICE_OBJECT_H
