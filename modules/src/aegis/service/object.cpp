/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/object.h"

#include "aegis/module.h"
#include "aegis/search/utils.h"
#include "aegis/service/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QWidget>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- ObjectParentCall ------------------------- */

ObjectParentCall::ObjectParentCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestParent) {}

ObjectParentCall::~ObjectParentCall() = default;

std::unique_ptr<ObjectParentCallData> ObjectParentCall::clone() const {
  return std::make_unique<ObjectParentCall>(getService(), getQueue());
}

ObjectParentCall::ProcessResult
ObjectParentCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  const auto response = parent(object);
  return {grpc::Status::OK, response};
}

ObjectParentCall::Response
ObjectParentCall::parent(const QObject *object) const {
  auto response = ObjectParentCall::Response{};

  const auto parent_query =
    object->parent() ? searcher().getQuery(object->parent()) : ObjectQuery{};
  response.set_parent(parent_query.toString().toStdString());


  return response;
}

/* ---------------------------- ObjectChildrenCall ------------------------ */

ObjectChildrenCall::ObjectChildrenCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestChildren) {}

ObjectChildrenCall::~ObjectChildrenCall() = default;

std::unique_ptr<ObjectChildrenCallData> ObjectChildrenCall::clone() const {
  return std::make_unique<ObjectChildrenCall>(getService(), getQueue());
}

ObjectChildrenCall::ProcessResult
ObjectChildrenCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  const auto response = children(object);
  return {grpc::Status::OK, response};
}

ObjectChildrenCall::Response
ObjectChildrenCall::children(const QObject *object) const {
  auto response = ObjectChildrenCall::Response{};

  for (const auto child : object->children()) {
    const auto child_query = searcher().getQuery(child);
    response.add_children(child_query.toString().toStdString());
  }

  return response;
}

/* --------------------------- ObjectInvokeMethodCall ----------------------- */

ObjectInvokeMethodCall::ObjectInvokeMethodCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestInvokeMethod) {}

ObjectInvokeMethodCall::~ObjectInvokeMethodCall() = default;

std::unique_ptr<ObjectInvokeMethodCallData>
ObjectInvokeMethodCall::clone() const {
  return std::make_unique<ObjectInvokeMethodCall>(getService(), getQueue());
}

ObjectInvokeMethodCall::ProcessResult
ObjectInvokeMethodCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return invoke(object, request.method(), request.arguments());
}

ObjectInvokeMethodCall::ProcessResult ObjectInvokeMethodCall::invoke(
  QObject *object, const std::string &method,
  const google::protobuf::RepeatedPtrField<google::protobuf::Value> &arguments)
  const {

  const auto meta_method = metaMethod(object, method);

  if (!meta_method.isValid()) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        QLatin1String("Method '%1' is unknown.")
          .arg(method.c_str())
          .toStdString()),
      {}};
  }

  if (meta_method.parameterCount() != arguments.size()) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        QLatin1String("Method '%1' parameters count is different from passed.")
          .arg(method.c_str())
          .toStdString()),
      {}};
  }

  auto parameters = QVariantList{};
  for (auto i = 0; i < meta_method.parameterCount(); ++i) {
    const auto parameter_meta_type = meta_method.parameterMetaType(i);
    auto parameter = convertIntoVariant(arguments[i]);

    if (!parameter.convert(parameter_meta_type)) {
      return {
        grpc::Status(
          grpc::StatusCode::INVALID_ARGUMENT,
          QLatin1String("Method '%1' parameter '%2' is incorrect.")
            .arg(method.c_str())
            .arg(parameter.toString())
            .toStdString()),
        {}};
    }

    parameters.push_back(std::move(parameter));
  }

  const auto _generic_arg = [&parameters](auto index) {
    return parameters.size() > index
             ? QGenericArgument(
                 parameters[index].typeName(), parameters[index].data())
             : QGenericArgument{};
  };

  meta_method.invoke(
    object, Qt::QueuedConnection, _generic_arg(0), _generic_arg(1),
    _generic_arg(2), _generic_arg(3), _generic_arg(4), _generic_arg(5),
    _generic_arg(6), _generic_arg(7), _generic_arg(8), _generic_arg(9));

  return {grpc::Status::OK, {}};
}

QMetaMethod ObjectInvokeMethodCall::metaMethod(
  const QObject *object, const std::string &name) const {
  const auto meta_object = object->metaObject();

  for (auto i = 0; i < meta_object->methodCount(); ++i) {
    const auto method = meta_object->method(i);
    if (method.name() == QByteArray::fromStdString(name)) { return method; }
  }

  return QMetaMethod{};
}

/* --------------------------- ObjectSetPropertyCall ---------------------- */

ObjectSetPropertyCall::ObjectSetPropertyCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestSetProperty) {}

ObjectSetPropertyCall::~ObjectSetPropertyCall() = default;

std::unique_ptr<ObjectSetPropertyCallData>
ObjectSetPropertyCall::clone() const {
  return std::make_unique<ObjectSetPropertyCall>(getService(), getQueue());
}

ObjectSetPropertyCall::ProcessResult
ObjectSetPropertyCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return setProperty(object, request.property(), request.value());
}

ObjectSetPropertyCall::ProcessResult ObjectSetPropertyCall::setProperty(
  QObject *object, const std::string &property,
  const google::protobuf::Value &value) const {

  const auto property_name = QByteArray::fromStdString(property);
  auto property_meta_type = object->property(property_name).metaType();

  if (property_meta_type.id() == QMetaType::UnknownType) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        QLatin1String("Property '%1' type is unknown.")
          .arg(property_name)
          .toStdString()),
      {}};
  }

  auto new_value = convertIntoVariant(value);
  if (!new_value.convert(property_meta_type)) {
    return {
      grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        QLatin1String("Property '%1' value '%2' is incorrect.")
          .arg(property_name)
          .arg(new_value.toString())
          .toStdString()),
      {}};
  }

  object->setProperty(property_name, new_value);
  return {grpc::Status::OK, {}};
}

/* --------------------------- ObjectDumpMethodsCall ---------------------- */

ObjectDumpMethodsCall::ObjectDumpMethodsCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestDumpMethods) {}

ObjectDumpMethodsCall::~ObjectDumpMethodsCall() = default;

std::unique_ptr<ObjectDumpMethodsCallData>
ObjectDumpMethodsCall::clone() const {
  return std::make_unique<ObjectDumpMethodsCall>(getService(), getQueue());
}

ObjectDumpMethodsCall::ProcessResult
ObjectDumpMethodsCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return {grpc::Status::OK, methods(object)};
}

aegis_proto::DumpMethodsResponse
ObjectDumpMethodsCall::methods(const QObject *object) const {
  auto response = ObjectDumpMethodsCall::Response{};

  auto meta_object = object->metaObject();
  for (auto i = 0; i < meta_object->methodCount(); ++i) {
    const auto method = meta_object->method(i);
    const auto method_name = method.name();

    auto parameters = QStringList{};
    for (auto j = 0; j < method.parameterCount(); ++j) {
      const auto parameter_type = method.parameterTypeName(j);

      parameters.append(QLatin1String("%1").arg(parameter_type));
    }

    response.add_methods(QLatin1String("%1(%2)")
                           .arg(method_name)
                           .arg(parameters.join(", "))
                           .toStdString());
  }

  return response;
}

/* -------------------------- ObjectDumpPropertiesCall ---------------------- */

ObjectDumpPropertiesCall::ObjectDumpPropertiesCall(
  aegis_proto::Object::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::Object::AsyncService::RequestDumpProperties) {}

ObjectDumpPropertiesCall::~ObjectDumpPropertiesCall() = default;

std::unique_ptr<ObjectDumpPropertiesCallData>
ObjectDumpPropertiesCall::clone() const {
  return std::make_unique<ObjectDumpPropertiesCall>(getService(), getQueue());
}

ObjectDumpPropertiesCall::ProcessResult
ObjectDumpPropertiesCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return {grpc::Status::OK, properties(object)};
}

aegis_proto::DumpPropertiesResponse
ObjectDumpPropertiesCall::properties(const QObject *object) const {
  auto response = ObjectDumpPropertiesCall::Response{};

  auto meta_object = object->metaObject();
  for (auto i = 0; i < meta_object->propertyCount(); ++i) {
    const auto name = meta_object->property(i).name();
    const auto value = object->property(name);

    auto new_properties = response.add_properties();
    new_properties->set_property(name);
    *new_properties->mutable_value() = convertIntoValue(value);
  }

  return response;
}

/* ------------------------------ ObjectService --------------------------- */

ObjectService::ObjectService() = default;

ObjectService::~ObjectService() = default;

void ObjectService::start(grpc::ServerCompletionQueue *queue) {
  auto parent_call = new ObjectParentCall(this, queue);
  auto children_call = new ObjectChildrenCall(this, queue);
  auto invoke_method_call = new ObjectInvokeMethodCall(this, queue);
  auto set_property_call = new ObjectSetPropertyCall(this, queue);
  auto dump_methods_call = new ObjectDumpMethodsCall(this, queue);
  auto dump_properties_call = new ObjectDumpPropertiesCall(this, queue);

  parent_call->proceed();
  children_call->proceed();
  invoke_method_call->proceed();
  set_property_call->proceed();
  dump_methods_call->proceed();
  dump_properties_call->proceed();
}

}// namespace aegis
