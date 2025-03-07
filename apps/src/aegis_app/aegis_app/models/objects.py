import json
import enum
import typing
import threading
import collections

from PySide6.QtCore import (
    Qt,
    QAbstractItemModel,
    QModelIndex,
    QMetaObject,
    Slot,
    Q_ARG,
)
from google.protobuf import empty_pb2
from aegis.aegis_pb2 import OptionalObject, Object

from aegis_app.client import Client


class ObjectNode:
    @staticmethod
    def create(
        query: str, parent: typing.Optional["ObjectNode"] = None
    ) -> "ObjectNode":
        query_data = json.loads(query)
        new_object_node = ObjectNode(
            id=query_data["memory_address"],
            name=query_data["path"].split(".")[-1],
            type=query_data["type"],
            path=query_data["path"],
            parent=parent,
        )
        return new_object_node

    def __init__(
        self,
        id: str,
        name: str,
        type: str,
        path: str,
        parent: typing.Optional["ObjectNode"] = None,
    ):
        self.id = id
        self.name = name
        self.type = type
        self.path = path
        self.parent = parent
        self.children = []

    def __eq__(self, other):
        if isinstance(other, ObjectNode):
            return self.id == other.id
        elif isinstance(other, str):
            return self.id == json.loads(other).get("memory_address", "")
        return NotImplemented

    def child(self, row: int) -> typing.Optional["ObjectNode"]:
        return self.children[row] if row < len(self.children) else None

    def row(self) -> int:
        if self.parent:
            return self.parent.children.index(self)
        return 0


class ObjectsModel(QAbstractItemModel):
    class Columns(enum.IntEnum):
        Name = 0
        Path = 1
        Type = 2

    def __init__(self, parent=None):
        super().__init__(parent)
        self._objects = []

    def rowCount(self, parent=QModelIndex()) -> int:
        if parent.isValid():
            parent_node = parent.internalPointer()
            return len(parent_node.children)

        return len(self._objects)

    def columnCount(self, parent=QModelIndex()) -> int:
        return len(ObjectsModel.Columns)

    def data(self, index, role=Qt.ItemDataRole.DisplayRole) -> typing.Any:
        if not index.isValid():
            return None

        node = index.internalPointer()
        if role == Qt.ItemDataRole.DisplayRole:
            match index.column():
                case ObjectsModel.Columns.Name:
                    return node.name
                case ObjectsModel.Columns.Type:
                    return node.type
                case ObjectsModel.Columns.Path:
                    return node.path

    def headerData(
        self, section, orientation, role=Qt.ItemDataRole.DisplayRole
    ) -> typing.Any:
        if (
            orientation == Qt.Orientation.Horizontal
            and role == Qt.ItemDataRole.DisplayRole
        ):
            match section:
                case ObjectsModel.Columns.Name:
                    return "Name"
                case ObjectsModel.Columns.Type:
                    return "Type"
                case ObjectsModel.Columns.Path:
                    return "Path"

    def index(self, row, column, parent=QModelIndex()) -> QModelIndex:
        if not self.hasIndex(row, column, parent):
            return QModelIndex()

        child_node = (
            parent.internalPointer().child(row)
            if parent.isValid()
            else self._objects[row]
        )

        if child_node:
            return self.createIndex(row, column, child_node)

        return QModelIndex()

    def parent(self, index) -> QModelIndex:
        if not index.isValid():
            return QModelIndex()

        child_node = index.internalPointer()
        parent_node = child_node.parent

        if child_node in self._objects or parent_node is None:
            return QModelIndex()

        return self.createIndex(parent_node.row(), 0, parent_node)

    def findItem(self, query: str, index=QModelIndex()) -> QModelIndex:
        node = index.internalPointer() if index.isValid() else None
        if node == query:
            return index

        for row in range(self.rowCount(index)):
            child_index = self.findItem(query, self.index(row, 0, index))
            if child_index.isValid():
                return child_index

        return QModelIndex()

    @Slot(str, QModelIndex)
    def createItem(self, query: str, parent_index=QModelIndex()) -> QModelIndex:
        parent_node = parent_index.internalPointer() if parent_index.isValid() else None
        node_container = parent_node.children if parent_node else self._objects
        new_node = ObjectNode.create(query, parent_node)
        row_count = self.rowCount(parent_index)

        self.beginInsertRows(parent_index, row_count, row_count)
        node_container.append(new_node)
        self.endInsertRows()

        return self.createIndex(new_node.row(), 0, new_node)

    @Slot(str, QModelIndex)
    def takeItem(
        self, query: str, parent_index=QModelIndex()
    ) -> typing.Optional["ObjectNode"]:
        parent_node = parent_index.internalPointer() if parent_index.isValid() else None
        node_container = parent_node.children if parent_node else self._objects
        find_index = self.findItem(query, parent_index)

        if not find_index.isValid() or self.parent(find_index) != parent_index:
            return None

        self.beginRemoveRows(parent_index, find_index.row(), find_index.row())
        node = node_container.pop(find_index.row())
        self.endRemoveRows()

        return node

    @Slot(str, QModelIndex)
    def updateItem(self, query: str, parent_index=QModelIndex()) -> QModelIndex:
        find_index = self.findItem(query, parent_index)
        if not find_index.isValid() or self.parent(find_index) != parent_index:
            return QModelIndex()

        node = find_index.internalPointer()
        assert node

        updated_node = ObjectNode.create(query)
        node.name = updated_node.name
        node.type = updated_node.type
        node.path = updated_node.path

        self.dataChanged.emit(
            find_index.sibling(find_index.row(), ObjectsModel.Columns.Name),
            find_index.sibling(find_index.row(), ObjectsModel.Columns.Path),
            [Qt.ItemDataRole.DisplayRole],
        )
        return find_index


class GRPCObjectsModel(ObjectsModel):
    def __init__(self, client: Client, parent=None):
        super().__init__(parent)
        self._client = client

        self.fetch_initial_state()

        self._watch_thread = threading.Thread(
            target=self.handle_tree_changes, daemon=True
        )
        self._watch_thread.start()

    def fetch_initial_state(self):
        response = self._client.object_stub.GetTree(OptionalObject())
        self.build_tree(response.nodes)

    def build_tree(self, object_nodes, parent_index=QModelIndex()):
        for object_node in object_nodes:
            node_index = self.createItem(object_node.object.query, parent_index)
            self.build_tree(object_node.nodes, node_index)

    def handle_tree_changes(self):
        for change in self._client.object_stub.ListenObjectChanges(empty_pb2.Empty()):
            if change.HasField("added"):
                QMetaObject.invokeMethod(
                    self,
                    "handle_object_added",
                    Qt.QueuedConnection,
                    Q_ARG(str, change.added.object.query),
                    Q_ARG(str, change.added.parent.query),
                )
            elif change.HasField("removed"):
                QMetaObject.invokeMethod(
                    self,
                    "handle_object_removed",
                    Qt.QueuedConnection,
                    Q_ARG(str, change.removed.object.query),
                )
            elif change.HasField("reparented"):
                QMetaObject.invokeMethod(
                    self,
                    "handle_object_reparented",
                    Qt.QueuedConnection,
                    Q_ARG(str, change.reparented.object.query),
                    Q_ARG(str, change.reparented.parent.query),
                )
            elif change.HasField("renamed"):
                QMetaObject.invokeMethod(
                    self,
                    "handle_object_renamed",
                    Qt.QueuedConnection,
                    Q_ARG(str, change.renamed.object.query),
                )

    @Slot(str)
    def handle_object_renamed(self, object):
        index = self.findItem(object)
        if not index.isValid():
            return

        parent_index = self.parent(index)
        self.updateItem(object, parent_index)

    @Slot(str, str)
    def handle_object_added(self, object, parent):
        parent_index = self.findItem(parent)
        self.createItem(object, parent_index)

    @Slot(str)
    def handle_object_removed(self, object):
        index = self.findItem(object)
        if not index.isValid():
            return

        parent_index = self.parent(index)
        self.takeItem(object, parent_index)

    @Slot(str, str)
    def handle_object_reparented(self, object, parent):
        index = self.findItem(object)
        if not index.isValid():
            return

        old_parent_index = self.parent(index)
        assert old_parent_index.isValid()

        new_parent_index = self.findItem(parent)
        assert new_parent_index.isValid()

        self.takeItem(object, old_parent_index)
        self.createItem(object, new_parent_index)
