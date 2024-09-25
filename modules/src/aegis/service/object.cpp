/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/object.h"

#include "aegis/module.h"
/* -------------------------------------------------------------------------- */

namespace {

static QVariant convertIntoVariant(const google::protobuf::Value& value) {
  switch (value.kind_case()) {
    case google::protobuf::Value::kStringValue:
      return QVariant::fromValue(QString::fromStdString(value.string_value()));

    case google::protobuf::Value::kNumberValue:
      return QVariant::fromValue(value.number_value());

    case google::protobuf::Value::kBoolValue:
      return QVariant::fromValue(value.bool_value());

    default:
      return QVariant{};
  }
}

static google::protobuf::Value convertIntoValue(const QVariant& variant) {
  auto value = google::protobuf::Value{};

  switch (variant.typeId()) {
    case QMetaType::Type::QString:
      value.set_string_value(variant.value<QString>().toStdString());
      break;

    case QMetaType::Type::Int:
      value.set_number_value(variant.toInt());
      break;

    case QMetaType::Type::Double:
      value.set_number_value(variant.toDouble());
      break;

    case QMetaType::Type::Bool:
      value.set_bool_value(variant.toBool());
      break;
  }

  return value;
}

}  // namespace

namespace aegis {

/* ------------------------------- ObjectFindCall --------------------------- */

ObjectFindCall::ObjectFindCall(aegis_proto::Object::AsyncService* service,
                               grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestFind) {}

ObjectFindCall::~ObjectFindCall() = default;

std::unique_ptr<ObjectFindCallData> ObjectFindCall::clone() const {
  return std::make_unique<ObjectFindCall>(getService(), getQueue());
}

ObjectFindCall::ProcessResult ObjectFindCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  const auto response = find(query);
  return {grpc::Status::OK, response};
}

ObjectFindCall::Response ObjectFindCall::find(const ObjectQuery& query) const {
  auto response = ObjectFindCall::Response{};
  for (const auto object : searcher().getObjects(query)) {
    response.add_objects(searcher().getQuery(object).toString().toStdString());
  }

  return response;
}

/* ------------------------------ ObjectParentCall -------------------------- */

ObjectParentCall::ObjectParentCall(aegis_proto::Object::AsyncService* service,
                                   grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestParent) {}

ObjectParentCall::~ObjectParentCall() = default;

std::unique_ptr<ObjectParentCallData> ObjectParentCall::clone() const {
  return std::make_unique<ObjectParentCall>(getService(), getQueue());
}

ObjectParentCall::ProcessResult ObjectParentCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  const auto response = find(query);
  return {grpc::Status::OK, response};
}

ObjectParentCall::Response ObjectParentCall::find(
    const ObjectQuery& query) const {
  auto response = ObjectParentCall::Response{};

  for (const auto object : searcher().getObjects(query)) {
    const auto object_query = searcher().getQuery(object);
    const auto parent_query = parent(object);

    auto added_object = response.add_objects();
    added_object->set_object(object_query.toString().toStdString());
    added_object->set_parent(parent_query.toString().toStdString());
  }

  return response;
}

ObjectQuery ObjectParentCall::parent(const QObject* object) const {
  const auto parent_query =
      object->parent() ? searcher().getQuery(object->parent()) : ObjectQuery{};

  return parent_query;
}

/* ----------------------------- ObjectChildrenCall ------------------------- */

ObjectChildrenCall::ObjectChildrenCall(
    aegis_proto::Object::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestChildren) {}

ObjectChildrenCall::~ObjectChildrenCall() = default;

std::unique_ptr<ObjectChildrenCallData> ObjectChildrenCall::clone() const {
  return std::make_unique<ObjectChildrenCall>(getService(), getQueue());
}

ObjectChildrenCall::ProcessResult ObjectChildrenCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  const auto response = find(query);
  return {grpc::Status::OK, response};
}

ObjectChildrenCall::Response ObjectChildrenCall::find(
    const ObjectQuery& query) const {
  auto response = ObjectChildrenCall::Response{};

  for (const auto object : searcher().getObjects(query)) {
    const auto object_query = searcher().getQuery(object);
    const auto children_queries = children(object);

    auto added_object = response.add_objects();
    added_object->set_object(object_query.toString().toStdString());

    for (const auto& children_query : children_queries) {
      added_object->add_children(children_query.toString().toStdString());
    }
  }

  return response;
}

QList<ObjectQuery> ObjectChildrenCall::children(const QObject* object) const {
  auto children = QList<ObjectQuery>{};
  for (const auto child : object->children()) {
    const auto child_query = searcher().getQuery(child);
    children.append(child_query);
  }

  return children;
}

/* --------------------------- ObjectInvokeMethodCall ----------------------- */

ObjectInvokeMethodCall::ObjectInvokeMethodCall(
    aegis_proto::Object::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestInvokeMethod) {}

ObjectInvokeMethodCall::~ObjectInvokeMethodCall() = default;

std::unique_ptr<ObjectInvokeMethodCallData> ObjectInvokeMethodCall::clone()
    const {
  return std::make_unique<ObjectInvokeMethodCall>(getService(), getQueue());
}

ObjectInvokeMethodCall::ProcessResult ObjectInvokeMethodCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  return invoke(query, request.method(), request.arguments());
}

ObjectInvokeMethodCall::ProcessResult ObjectInvokeMethodCall::invoke(
    const ObjectQuery& query, const std::string& method,
    const google::protobuf::RepeatedPtrField<google::protobuf::Value>&
        arguments) const {
  const auto objects = searcher().getObjects(query);
  if (objects.empty()) return {grpc::Status::OK, {}};

  const auto meta_method = metaMethod(objects.front(), method);
  for (const auto object : objects) {
    const auto current_meta_method = metaMethod(object, method);

    if (!current_meta_method.isValid()) {
      return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                           QLatin1String("Method '%1' is unknown.")
                               .arg(method.c_str())
                               .toStdString()),
              {}};
    }

    if (current_meta_method != meta_method) {
      return {
          grpc::Status(
              grpc::StatusCode::INVALID_ARGUMENT,
              QLatin1String("Method '%1' is different for objects from query.")
                  .arg(method.c_str())
                  .toStdString()),
          {}};
    }
  }

  if (meta_method.parameterCount() != arguments.size()) {
    return {grpc::Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                QLatin1String(
                    "Method '%1' parameters count is different from passed.")
                    .arg(method.c_str())
                    .toStdString()),
            {}};
  }

  auto parameters = std::array<QGenericArgument, 10>{};
  for (auto i = 0; i < meta_method.parameterCount(); ++i) {
    const auto parameter_meta_type = meta_method.parameterMetaType(i);
    auto parameter = convertIntoVariant(arguments[i]);

    if (!parameter.convert(parameter_meta_type)) {
      return {
          grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                       QLatin1String("Method '%1' parameter '%2' is incorrect.")
                           .arg(method.c_str())
                           .arg(parameter.toString())
                           .toStdString()),
          {}};
    }

    parameters[i] = QGenericArgument(parameter.typeName(), parameter.data());
  }

  for (auto object : objects) {
    meta_method.invoke(object, Qt::DirectConnection, parameters[0],
                       parameters[1], parameters[2], parameters[3],
                       parameters[4], parameters[5], parameters[6],
                       parameters[7], parameters[8], parameters[9]);
  }

  return {grpc::Status::OK, {}};
}

QMetaMethod ObjectInvokeMethodCall::metaMethod(const QObject* object,
                                               const std::string& name) const {
  const auto meta_object = object->metaObject();

  for (auto i = 0; i < meta_object->methodCount(); ++i) {
    const auto method = meta_object->method(i);
    if (method.name() == QByteArray::fromStdString(name)) {
      return method;
    }
  }

  return QMetaMethod{};
}

/* ---------------------------- ObjectSetPropertyCall ----------------------- */

ObjectSetPropertyCall::ObjectSetPropertyCall(
    aegis_proto::Object::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestSetProperty) {}

ObjectSetPropertyCall::~ObjectSetPropertyCall() = default;

std::unique_ptr<ObjectSetPropertyCallData> ObjectSetPropertyCall::clone()
    const {
  return std::make_unique<ObjectSetPropertyCall>(getService(), getQueue());
}

ObjectSetPropertyCall::ProcessResult ObjectSetPropertyCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  return setProperty(query, request.property(), request.value());
}

ObjectSetPropertyCall::ProcessResult ObjectSetPropertyCall::setProperty(
    const ObjectQuery& query, const std::string& property,
    const google::protobuf::Value& value) const {
  const auto objects = searcher().getObjects(query);
  if (objects.empty()) return {grpc::Status::OK, {}};

  const auto property_name = QByteArray::fromStdString(property);
  auto property_meta_type = objects.front()->property(property_name).metaType();
  for (const auto object : objects) {
    const auto current_property_type =
        object->property(property_name).metaType();

    if (current_property_type.id() == QMetaType::UnknownType) {
      return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                           QLatin1String("Property '%1' type is unknown.")
                               .arg(property_name)
                               .toStdString()),
              {}};
    }

    if (current_property_type != property_meta_type) {
      return {grpc::Status(
                  grpc::StatusCode::INVALID_ARGUMENT,
                  QLatin1String(
                      "Property '%1' type is different for objects from query.")
                      .arg(property_name)
                      .toStdString()),
              {}};
    }
  }

  auto new_value = convertIntoVariant(value);
  if (!new_value.convert(property_meta_type)) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         QLatin1String("Property '%1' value '%2' is incorrect.")
                             .arg(property_name)
                             .arg(new_value.toString())
                             .toStdString()),
            {}};
  }

  for (const auto object : objects) {
    object->setProperty(property_name, new_value);
  }

  return {grpc::Status::OK, {}};
}

/* ---------------------------- ObjectDumpMethodsCall ----------------------- */

ObjectDumpMethodsCall::ObjectDumpMethodsCall(
    aegis_proto::Object::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestDumpMethods) {}

ObjectDumpMethodsCall::~ObjectDumpMethodsCall() = default;

std::unique_ptr<ObjectDumpMethodsCallData> ObjectDumpMethodsCall::clone()
    const {
  return std::make_unique<ObjectDumpMethodsCall>(getService(), getQueue());
}

ObjectDumpMethodsCall::ProcessResult ObjectDumpMethodsCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  return dump(query);
}

ObjectDumpMethodsCall::ProcessResult ObjectDumpMethodsCall::dump(
    const ObjectQuery& query) const {
  auto response = ObjectDumpMethodsCall::Response{};
  for (const auto object : searcher().getObjects(query)) {
    *response.add_objects() = methods(object);
  }

  return {grpc::Status::OK, response};
}

aegis_proto::ObjectMethods ObjectDumpMethodsCall::methods(
    const QObject* object) const {
  auto object_methods = aegis_proto::ObjectMethods{};
  object_methods.set_object(
      searcher().getQuery(object).toString().toStdString());

  auto meta_object = object->metaObject();
  for (auto i = 0; i < meta_object->methodCount(); ++i) {
    const auto method = meta_object->method(i);
    const auto method_name = method.name();

    auto parameters = QStringList{};
    for (auto j = 0; j < method.parameterCount(); ++j) {
      const auto parameter_type = method.parameterTypeName(j);

      parameters.append(QLatin1String("%1").arg(parameter_type));
    }

    object_methods.add_methods(QLatin1String("%1(%2)")
                                   .arg(method_name)
                                   .arg(parameters.join(", "))
                                   .toStdString());
  }

  return object_methods;
}

/* -------------------------- ObjectDumpPropertiesCall ---------------------- */

ObjectDumpPropertiesCall::ObjectDumpPropertiesCall(
    aegis_proto::Object::AsyncService* service,
    grpc::ServerCompletionQueue* queue)
    : CallData(service, queue, CallTag{this},
               &aegis_proto::Object::AsyncService::RequestDumpProperties) {}

ObjectDumpPropertiesCall::~ObjectDumpPropertiesCall() = default;

std::unique_ptr<ObjectDumpPropertiesCallData> ObjectDumpPropertiesCall::clone()
    const {
  return std::make_unique<ObjectDumpPropertiesCall>(getService(), getQueue());
}

ObjectDumpPropertiesCall::ProcessResult ObjectDumpPropertiesCall::process(
    const Request& request) const {
  const auto query =
      ObjectQuery::fromString(QString::fromStdString(request.object()));
  if (!query.isValid()) {
    return {grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                         "Object argument has incorrect format"),
            {}};
  }

  return dump(query);
}

ObjectDumpPropertiesCall::ProcessResult ObjectDumpPropertiesCall::dump(
    const ObjectQuery& query) const {
  auto response = ObjectDumpPropertiesCall::Response{};
  for (const auto object : searcher().getObjects(query)) {
    *response.add_objects() = properties(object);
  }

  return {grpc::Status::OK, response};
}

aegis_proto::ObjectProperties ObjectDumpPropertiesCall::properties(
    const QObject* object) const {
  auto object_properties = aegis_proto::ObjectProperties{};
  object_properties.set_object(
      searcher().getQuery(object).toString().toStdString());

  auto meta_object = object->metaObject();
  for (auto i = 0; i < meta_object->propertyCount(); ++i) {
    const auto name = meta_object->property(i).name();
    const auto value = object->property(name);

    auto new_properties = object_properties.add_properties();
    new_properties->set_property(name);
    *new_properties->mutable_value() = convertIntoValue(value);
  }

  return object_properties;
}

/* ------------------------------- ObjectService ---------------------------- */

ObjectService::ObjectService() = default;

ObjectService::~ObjectService() = default;

void ObjectService::start(grpc::ServerCompletionQueue* queue) {
  auto find_call = new ObjectFindCall(this, queue);
  auto parent_call = new ObjectParentCall(this, queue);
  auto children_call = new ObjectChildrenCall(this, queue);
  auto invoke_method_call = new ObjectInvokeMethodCall(this, queue);
  auto set_property_call = new ObjectSetPropertyCall(this, queue);
  auto dump_methods_call = new ObjectDumpMethodsCall(this, queue);
  auto dump_properties_call = new ObjectDumpPropertiesCall(this, queue);

  find_call->proceed();
  parent_call->proceed();
  children_call->proceed();
  invoke_method_call->proceed();
  set_property_call->proceed();
  dump_methods_call->proceed();
  dump_properties_call->proceed();
}

}  // namespace aegis
