/* ----------------------------------- Local -------------------------------- */
#include "aegis/command/parent.h"

#include "aegis/manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QCursor>
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------- ObjectParentMessage ----------------------- */

bool ObjectParentMessage::operator==(const ObjectParentMessage& other) const {
  return object == other.object && parent == other.parent;
}

bool ObjectParentMessage::operator!=(const ObjectParentMessage& other) const {
  return object != other.object || parent != other.parent;
}

/* ------------------------------ ParentCommand ----------------------------- */

ParentCommand::ParentCommand() : Command(QLatin1String("Parent")) {
  m_parser.addOptions({{{"q", "query"},
                        "ObjectQuery that identifies the object whose parent "
                        "we are looking for",
                        "query"}});
  m_required_options.append("query");
}

ParentCommand::~ParentCommand() = default;

QByteArray ParentCommand::exec() {
  const auto query_str = m_parser.value("query");
  const auto query = ObjectQuery::fromString(query_str);
  if (!query.isValid()) {
    auto error = Response<>(ErrorMessage(
        getError(), QLatin1String("ObjectQuery has incorrect format.")));
    return serializer()->serialize(error);
  }

  return serializer()->serialize(find(query));
}

Response<ObjectsParentMessage> ParentCommand::find(
    const ObjectQuery& query) const {
  const auto objects = searcher()->getObjects(query);

  auto message = ObjectsParentMessage{};
  for (const auto object : objects) {
    const auto object_id = searcher()->getQuery(object);
    const auto parent_id = getParent(object);

    message.objects.append(
        ObjectParentMessage{object_id.toString(), parent_id.toString()});
  }

  return message;
}

ObjectQuery ParentCommand::getParent(const QObject* object) const {
  const auto parent_id =
      object->parent() ? searcher()->getQuery(object->parent()) : ObjectQuery{};

  return parent_id;
}

}  // namespace aegis
