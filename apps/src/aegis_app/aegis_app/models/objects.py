import json
import enum
import typing
import threading
from PySide6.QtCore import Qt, QAbstractItemModel, QModelIndex, QTimer
from google.protobuf import empty_pb2
from aegis.aegis_pb2 import OptionalObjectRequest

from aegis_app.client import Client


class ObjectNode:
    def __init__(
        self,
        query: str,
        name: str,
        type: str,
        path: str,
        address: str,
        parent: typing.Optional["ObjectNode"] = None,
    ):
        self._query = query
        self._name = name
        self._type = type
        self._path = path
        self._address = address
        self._parent = parent
        self._children = []

    def append_child(self, child: "ObjectNode"):
        self._children.append(child)

    def child(self, row: int) -> typing.Optional["ObjectNode"]:
        return self._children[row] if row < len(self._children) else None

    def child_count(self) -> int:
        return len(self._children)

    def row(self) -> int:
        if self._parent:
            return self._parent._children.index(self)
        return 0

    def query(self) -> typing.Any:
        return self._query

    def name(self) -> typing.Any:
        return self._name

    def path(self) -> typing.Any:
        return self._path

    def type(self) -> typing.Any:
        return self._type

    def address(self) -> typing.Any:
        return self._address


def create_object_node(
    query: str, parent: typing.Optional["ObjectNode"] = None
) -> ObjectNode:
    query_data = json.loads(query)
    return ObjectNode(
        query=query,
        name=query_data["path"].split(".")[-1],
        type=query_data["type"],
        path=query_data["path"],
        address=query_data["memory_address"],
        parent=parent,
    )


class ObjectsModel(QAbstractItemModel):
    class Columns(enum.IntEnum):
        Name = 0
        Path = 1
        Type = 2
        Address = 3

    def __init__(self, parent=None):
        super().__init__(parent)
        self._objects = []

    def rowCount(self, parent=QModelIndex()):
        if parent.isValid():
            parent_node = parent.internalPointer()
            return parent_node.child_count()

        return len(self._objects)

    def columnCount(self, parent=QModelIndex()):
        return len(ObjectsModel.Columns)

    def data(self, index, role=Qt.ItemDataRole.DisplayRole):
        if not index.isValid():
            return None

        node = index.internalPointer()
        if role == Qt.ItemDataRole.DisplayRole:
            match index.column():
                case ObjectsModel.Columns.Name:
                    return node.name()
                case ObjectsModel.Columns.Type:
                    return node.type()
                case ObjectsModel.Columns.Path:
                    return node.path()
                case ObjectsModel.Columns.Address:
                    return node.address()

    def headerData(self, section, orientation, role=Qt.ItemDataRole.DisplayRole):
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
                case ObjectsModel.Columns.Address:
                    return "Address"

    def index(self, row, column, parent=QModelIndex()):
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

    def parent(self, index):
        if not index.isValid():
            return QModelIndex()

        child_node = index.internalPointer()
        parent_node = child_node._parent

        if child_node in self._objects or parent_node is None:
            return QModelIndex()

        return self.createIndex(parent_node.row(), 0, parent_node)

    def addItem(self, object_id: str, parent_index=QModelIndex()):
        if parent_index.isValid():
            parent_node = parent_index.internalPointer()
        else:
            parent_node = None

        new_node = ObjectNode(object_id, parent_node)

        if parent_node:
            self.beginInsertRows(
                parent_index, parent_node.child_count(), parent_node.child_count()
            )
            parent_node.append_child(new_node)
            self.endInsertRows()
        else:
            self.beginInsertRows(QModelIndex(), len(self.objects), len(self.objects))
            self._objects.append(new_node)
            self.endInsertRows()

        return self.createIndex(new_node.row(), 0, new_node)


class GRPCObjectsModel(ObjectsModel):
    def __init__(self, client: Client, parent=None):
        super().__init__(parent)
        self._client = client

        self.fetch_initial_state()
        self._watch_thread = threading.Thread(target=self.tree_changed, daemon=True)
        self._watch_thread.start()

    def fetch_initial_state(self):
        response = self._client.object_stub.Tree(OptionalObjectRequest())

        self.beginResetModel()
        self._objects = self.build_tree(response.nodes, None)
        self.endResetModel()

    def build_tree(self, objects, parent):
        nodes = []
        for object in objects:
            node = create_object_node(object.object, parent)

            children = self.build_tree(object.nodes, node)
            for child in children:
                node.append_child(child)

            nodes.append(node)
        return nodes

    def tree_changed(self):
        for change in self._client.object_stub.ListenObjectChanges(empty_pb2.Empty()):
            print(f"tree_changed {change}")
