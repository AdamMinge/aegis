/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/set_property.h"

#include "aegis/manager.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- SetPropertyCommand ------------------------- */

SetPropertyCommand::SetPropertyCommand()
    : Command(QLatin1String("SetProperty")) {
  m_parser.addOptions({
      {{"q", "query"},
       "ObjectQuery that identifies the objects we are dumping for",
       "query"},
      {{"n", "name"}, "Name that identifies the property", "name"},
      {{"v", "value"}, "Value we are setting for the property", "value"},
  });
  m_required_options.append({"query", "name", "value"});
}

SetPropertyCommand::~SetPropertyCommand() = default;

QByteArray SetPropertyCommand::exec() {
  const auto query_str = m_parser.value("query");
  const auto query = ObjectQuery::fromString(query_str);
  if (!query.isValid()) {
    auto error = Response<>(ErrorMessage(
        getError(), QLatin1String("ObjectQuery has incorrect format.")));
    return serializer()->serialize(error);
  }

  const auto name = m_parser.value("name");
  const auto value = m_parser.value("value");

  return serializer()->serialize(setProperty(query, name, value));
}

Response<> SetPropertyCommand::setProperty(const ObjectQuery& query,
                                           const QString& name,
                                           const QString& value) {
  const auto objects = searcher()->getObjects(query);
  if (objects.empty()) return EmptyMessage{};

  const auto property_name = name.toUtf8().data();
  auto property_type = objects.front()->property(property_name).metaType();
  for (const auto object : objects) {
    const auto current_property_type =
        object->property(property_name).metaType();

    if (current_property_type.id() == QMetaType::UnknownType) {
      auto error = Response<>(ErrorMessage(
          getError(),
          QLatin1String("Property '%1' type is unknown.").arg(name)));
      return error;
    }

    if (current_property_type != property_type) {
      auto error = Response<>(ErrorMessage(
          getError(),
          QLatin1String(
              "Property '%1' type is different for objects from query.")
              .arg(name)));
      return error;
    }
  }

  auto new_value = QVariant::fromValue(value);
  if (!new_value.convert(property_type)) {
    auto error = Response<>(ErrorMessage(
        getError(),
        QLatin1String("Property value '%1' is incorrect.").arg(value)));
    return error;
  }

  for (const auto object : objects) {
    object->setProperty(property_name, new_value);
  }

  return EmptyMessage{};
}

}  // namespace aegis