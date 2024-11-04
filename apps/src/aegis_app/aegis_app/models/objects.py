import typing
from PySide6.QtCore import Qt, QAbstractItemModel, QModelIndex


class ObjectNode:
    def __init__(self, object_id: str, parent: typing.Optional["ObjectNode"] = None):
        self.object_id = object_id
        self.parent = parent
        self.children = []

    def append_child(self, child: "ObjectNode"):
        self.children.append(child)

    def child(self, row: int) -> typing.Optional["ObjectNode"]:
        return self.children[row] if row < len(self.children) else None

    def child_count(self) -> int:
        return len(self.children)

    def row(self) -> int:
        if self.parent:
            return self.parent.children.index(self)
        return 0

    def data(self) -> typing.Any:
        return self.object_id


class ObjectsModel(QAbstractItemModel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.objects = []

    def rowCount(self, parent=QModelIndex()):
        if parent.isValid():
            parent_node = parent.internalPointer()
            return parent_node.child_count()

        return len(self.objects)

    def columnCount(self, parent=QModelIndex()):
        return 1

    def data(self, index, role=Qt.ItemDataRole.DisplayRole):
        if not index.isValid():
            return None

        node = index.internalPointer()
        if role == Qt.ItemDataRole.DisplayRole:
            return node.data()

    def headerData(self, section, orientation, role=Qt.ItemDataRole.DisplayRole):
        if (
            orientation == Qt.Orientation.Horizontal
            and role == Qt.ItemDataRole.DisplayRole
        ):
            return "Object"

    def index(self, row, column, parent=QModelIndex()):
        if not self.hasIndex(row, column, parent):
            return QModelIndex()

        child_node = (
            parent.internalPointer().child(row)
            if parent.isValid()
            else self.objects[row]
        )

        if child_node:
            return self.createIndex(row, column, child_node)

        return QModelIndex()

    def parent(self, index):
        if not index.isValid():
            return QModelIndex()

        child_node = index.internalPointer()
        parent_node = child_node.parent

        if parent_node in self.objects or parent_node is None:
            return QModelIndex()

        return self.createIndex(parent_node.row(), 0, parent_node)
