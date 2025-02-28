/* ----------------------------------- Local -------------------------------- */
#include "aegis/service/object.h"

#include "aegis/module.h"
#include "aegis/observe/action.h"
#include "aegis/observe/observer.h"
#include "aegis/search/utils.h"
#include "aegis/service/utils.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QWidget>
/* --------------------------------- Standard ------------------------------- */
#include <queue>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* --------------------------- ObservedActionsMapper ------------------------ */

class ObservedActionsMapper {
public:
  aegis_proto::ObjectChange
  operator()(const ObservedAction::ObjectAdded &action) const {
    aegis_proto::ObjectChange response;
    auto added = response.mutable_added();
    added->mutable_object()->set_query(action.object.toString().toStdString());
    added->mutable_parent()->set_query(action.parent.toString().toStdString());
    return response;
  }

  aegis_proto::ObjectChange
  operator()(const ObservedAction::ObjectRemoved &action) const {
    aegis_proto::ObjectChange response;
    auto removed = response.mutable_removed();
    removed->mutable_object()->set_query(
      action.object.toString().toStdString());
    removed->mutable_parent()->set_query(
      action.parent.toString().toStdString());
    return response;
  }

  aegis_proto::ObjectChange
  operator()(const ObservedAction::ObjectReparented &action) const {
    aegis_proto::ObjectChange response;
    auto reparented = response.mutable_reparented();
    reparented->mutable_object()->set_query(
      action.object.toString().toStdString());
    reparented->mutable_parent()->set_query(
      action.parent.toString().toStdString());
    return response;
  }
};

/* ------------------------------ ObjectGetTreeCall -------------------------- */

ObjectGetTreeCall::ObjectGetTreeCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestGetTree) {}

ObjectGetTreeCall::~ObjectGetTreeCall() = default;

std::unique_ptr<ObjectGetTreeCallData> ObjectGetTreeCall::clone() const {
  return std::make_unique<ObjectGetTreeCall>(getService(), getQueue());
}

ObjectGetTreeCall::ProcessResult
ObjectGetTreeCall::process(const Request &request) const {
  auto objects = QObjectList{};

  if (request.has_query()) {
    auto query =
      ObjectQuery::fromString(QString::fromStdString(request.query()));
    auto [status, object] = tryGetSingleObject(query);
    if (!status.ok()) return {status, {}};
    objects.append(object);
  } else {
    objects.append(getTopLevelObjects());
  }

  const auto response = tree(objects);
  return {grpc::Status::OK, response};
}

ObjectGetTreeCall::Response
ObjectGetTreeCall::tree(const QObjectList &objects) const {
  auto response = ObjectGetTreeCall::Response{};

  auto objectsToProcess =
    std::queue<std::pair<QObject *, aegis_proto::ObjectNode *>>{};
  for (const auto object : objects) {
    objectsToProcess.push(std::make_pair(object, response.add_nodes()));
  }

  while (!objectsToProcess.empty()) {
    auto objectToProcess = objectsToProcess.front();
    auto object = objectToProcess.first;
    auto object_children = objectToProcess.second;
    objectsToProcess.pop();

    const auto object_query = searcher().getQuery(object);
    object_children->mutable_object()->set_query(
      object_query.toString().toStdString());

    for (const auto child : object->children()) {
      objectsToProcess.push(
        std::make_pair(child, object_children->add_nodes()));
    }
  }


  return response;
}

/* ------------------------------ ObjectFindCall -------------------------- */

ObjectFindCall::ObjectFindCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestFind) {}

ObjectFindCall::~ObjectFindCall() = default;

std::unique_ptr<ObjectFindCallData> ObjectFindCall::clone() const {
  return std::make_unique<ObjectFindCall>(getService(), getQueue());
}

ObjectFindCall::ProcessResult
ObjectFindCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.query()));

  auto [status, objects] = tryGetObjects(query);
  if (!status.ok()) return {status, {}};

  const auto response = find(objects);
  return {grpc::Status::OK, response};
}

ObjectFindCall::Response
ObjectFindCall::find(const QObjectList &objects) const {
  auto response = ObjectFindCall::Response{};
  for (const auto object : objects) {
    const auto query = searcher().getQuery(object);
    response.add_objects()->set_query(query.toString().toStdString());
  }

  return response;
}

/* ----------------------------- ObjectParentCall ------------------------- */

ObjectParentCall::ObjectParentCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestGetParent) {}

ObjectParentCall::~ObjectParentCall() = default;

std::unique_ptr<ObjectParentCallData> ObjectParentCall::clone() const {
  return std::make_unique<ObjectParentCall>(getService(), getQueue());
}

ObjectParentCall::ProcessResult
ObjectParentCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.query()));

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
  response.set_query(parent_query.toString().toStdString());


  return response;
}

/* ---------------------------- ObjectChildrenCall ------------------------ */

ObjectChildrenCall::ObjectChildrenCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestGetChildren) {}

ObjectChildrenCall::~ObjectChildrenCall() = default;

std::unique_ptr<ObjectChildrenCallData> ObjectChildrenCall::clone() const {
  return std::make_unique<ObjectChildrenCall>(getService(), getQueue());
}

ObjectChildrenCall::ProcessResult
ObjectChildrenCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.query()));

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
    response.add_objects()->set_query(child_query.toString().toStdString());
  }

  return response;
}

/* --------------------------- ObjectCallMethodCall ----------------------- */

ObjectCallMethodCall::ObjectCallMethodCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestCallMethod) {}

ObjectCallMethodCall::~ObjectCallMethodCall() = default;

std::unique_ptr<ObjectCallMethodCallData> ObjectCallMethodCall::clone() const {
  return std::make_unique<ObjectCallMethodCall>(getService(), getQueue());
}

ObjectCallMethodCall::ProcessResult
ObjectCallMethodCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object().query()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return invoke(object, request.method(), request.arguments());
}

ObjectCallMethodCall::ProcessResult ObjectCallMethodCall::invoke(
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

QMetaMethod ObjectCallMethodCall::metaMethod(
  const QObject *object, const std::string &name) const {
  const auto meta_object = object->metaObject();

  for (auto i = 0; i < meta_object->methodCount(); ++i) {
    const auto method = meta_object->method(i);
    if (method.name() == QByteArray::fromStdString(name)) { return method; }
  }

  return QMetaMethod{};
}

/* ------------------------- ObjectUpdatePropertyCall --------------------- */

ObjectUpdatePropertyCall::ObjectUpdatePropertyCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestUpdateProperty) {}

ObjectUpdatePropertyCall::~ObjectUpdatePropertyCall() = default;

std::unique_ptr<ObjectUpdatePropertyCallData>
ObjectUpdatePropertyCall::clone() const {
  return std::make_unique<ObjectUpdatePropertyCall>(getService(), getQueue());
}

ObjectUpdatePropertyCall::ProcessResult
ObjectUpdatePropertyCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.object().query()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return setProperty(object, request.property(), request.value());
}

ObjectUpdatePropertyCall::ProcessResult ObjectUpdatePropertyCall::setProperty(
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

/* --------------------------- ObjectGetMethodsCall ---------------------- */

ObjectGetMethodsCall::ObjectGetMethodsCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestGetMethods) {}

ObjectGetMethodsCall::~ObjectGetMethodsCall() = default;

std::unique_ptr<ObjectGetMethodsCallData> ObjectGetMethodsCall::clone() const {
  return std::make_unique<ObjectGetMethodsCall>(getService(), getQueue());
}

ObjectGetMethodsCall::ProcessResult
ObjectGetMethodsCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.query()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return {grpc::Status::OK, methods(object)};
}

ObjectGetMethodsCall::Response
ObjectGetMethodsCall::methods(const QObject *object) const {
  auto response = ObjectGetMethodsCall::Response{};

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

/* -------------------------- ObjectGetPropertiesCall ---------------------- */

ObjectGetPropertiesCall::ObjectGetPropertiesCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : CallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestGetProperties) {}

ObjectGetPropertiesCall::~ObjectGetPropertiesCall() = default;

std::unique_ptr<ObjectGetPropertiesCallData>
ObjectGetPropertiesCall::clone() const {
  return std::make_unique<ObjectGetPropertiesCall>(getService(), getQueue());
}

ObjectGetPropertiesCall::ProcessResult
ObjectGetPropertiesCall::process(const Request &request) const {
  const auto query =
    ObjectQuery::fromString(QString::fromStdString(request.query()));

  auto [status, object] = tryGetSingleObject(query);
  if (!status.ok()) return {status, {}};

  return {grpc::Status::OK, properties(object)};
}

ObjectGetPropertiesCall::Response
ObjectGetPropertiesCall::properties(const QObject *object) const {
  auto response = ObjectGetPropertiesCall::Response{};

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

/* ------------------------ ObjectListenObjectChangesCall ----------------- */

ObjectListenObjectChangesCall::ObjectListenObjectChangesCall(
  aegis_proto::ObjectService::AsyncService *service,
  grpc::ServerCompletionQueue *queue)
    : StreamCallData(
        service, queue, CallTag{this},
        &aegis_proto::ObjectService::AsyncService::RequestListenObjectChanges),
      m_observer(std::make_unique<ObjectObserver>()),
      m_observer_queue(std::make_unique<ObjectObserverQueue>()),
      m_mapper(std::make_unique<ObservedActionsMapper>()) {

  m_observer_queue->setObserver(m_observer.get());
  m_observer->moveToThread(qApp->thread());
  m_observer->start();
}

ObjectListenObjectChangesCall::~ObjectListenObjectChangesCall() = default;

ObjectListenObjectChangesCall::ProcessResult
ObjectListenObjectChangesCall::process(const Request &request) const {
  const auto observer_action = m_observer_queue->waitPopAction();
  const auto response = observer_action.visit(*m_mapper);

  return response;
}

std::unique_ptr<ObjectListenObjectChangesCallData>
ObjectListenObjectChangesCall::clone() const {
  return std::make_unique<ObjectListenObjectChangesCall>(
    getService(), getQueue());
}

/* ------------------------------ ObjectService --------------------------- */

ObjectService::ObjectService() = default;

ObjectService::~ObjectService() = default;

void ObjectService::start(grpc::ServerCompletionQueue *queue) {
  auto get_tree_call = new ObjectGetTreeCall(this, queue);
  auto find_call = new ObjectFindCall(this, queue);
  auto parent_call = new ObjectParentCall(this, queue);
  auto children_call = new ObjectChildrenCall(this, queue);
  auto call_method_call = new ObjectCallMethodCall(this, queue);
  auto update_property_call = new ObjectUpdatePropertyCall(this, queue);
  auto get_methods_call = new ObjectGetMethodsCall(this, queue);
  auto get_properties_call = new ObjectGetPropertiesCall(this, queue);
  auto listen_object_changes = new ObjectListenObjectChangesCall(this, queue);

  get_tree_call->proceed();
  find_call->proceed();
  parent_call->proceed();
  children_call->proceed();
  call_method_call->proceed();
  update_property_call->proceed();
  get_methods_call->proceed();
  get_properties_call->proceed();
  listen_object_changes->proceed();
}

}// namespace aegis
