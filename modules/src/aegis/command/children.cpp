/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/children.h"

#include "aegis/manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QCursor>
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------- ObjectChildrenMessage ----------------------- */

bool ObjectChildrenMessage::operator==(
    const ObjectChildrenMessage& other) const {
  return object == other.object && children == other.children;
}

bool ObjectChildrenMessage::operator!=(
    const ObjectChildrenMessage& other) const {
  return object != other.object || children != other.children;
}

/* ------------------------------ ChildrenCommand --------------------------- */

ChildrenCommand::ChildrenCommand() : Command(QLatin1String("Children")) {
  m_parser.addOptions(
      {{{"q", "query"},
        "ObjectQuery that identifies the object whose children we are looking "
        "for",
        "query"}});
  m_required_options.append("query");
}

ChildrenCommand::~ChildrenCommand() = default;

QByteArray ChildrenCommand::exec() {
  const auto query_str = m_parser.value("query");
  const auto query = ObjectQuery::fromString(query_str);
  if (!query.isValid()) {
    auto error = Response<>(ErrorMessage(
        getError(), QLatin1String("ObjectQuery has incorrect format.")));
    return serializer()->serialize(error);
  }

  return serializer()->serialize(find(query));
}

Response<ObjectsChildrenMessage> ChildrenCommand::find(
    const ObjectQuery& query) const {
  const auto objects = searcher()->getObjects(query);

  auto message = ObjectsChildrenMessage{};
  for (const auto object : objects) {
    const auto object_id = searcher()->getId(object);
    const auto children_ids = getChildren(object);

    auto children = QStringList{};
    std::transform(children_ids.begin(), children_ids.end(),
                   std::back_inserter(children),
                   [](const auto& child) { return child.toString(); });

    message.objects.append(
        ObjectChildrenMessage{object_id.toString(), children});
  }

  return message;
}

QList<ObjectQuery> ChildrenCommand::getChildren(const QObject* object) const {
  auto children = QList<ObjectQuery>{};
  for (const auto child : object->children()) {
    const auto child_id = searcher()->getId(child);
    children.append(child_id);
  }

  return children;
}

}  // namespace aegis