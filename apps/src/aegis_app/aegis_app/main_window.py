import dataclasses

from PySide6.QtWidgets import (
    QMainWindow,
    QSizePolicy,
    QDockWidget,
    QWidget,
    QVBoxLayout,
    QTableView,
    QTreeView,
    QHeaderView,
)
from PySide6.QtCore import Slot, Qt

from pyside6_utils.models import DataclassModel
from pyside6_utils.widgets import DataClassTreeView
from pyside6_utils.widgets.delegates import DataclassEditorsDelegate

from aegis_app.client import Client
from aegis_app.models import MethodsModel, ObjectsModel


class ObjectsDock(QDockWidget):
    def __init__(self):
        super().__init__("Objects")
        self._init_ui()

    def _init_ui(self):
        self._model = ObjectsModel()
        self._view = QTreeView()
        self._view.setModel(self._model)
        self._view.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self._view.setAlternatingRowColors(True)
        self._view.setHeaderHidden(True)

        container = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(self._view)
        container.setLayout(layout)
        self.setWidget(container)


class PropertiesDock(QDockWidget):
    def __init__(self):
        super().__init__("Properties")
        self._init_ui()

    def _init_ui(self):
        @dataclasses.dataclass
        class EmptyDataclass:
            pass

        self._model = DataclassModel(EmptyDataclass())
        self._view = DataClassTreeView()
        self._view.setModel(self._model)
        self._view.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self._view.setItemDelegate(DataclassEditorsDelegate())
        self._view.setAlternatingRowColors(True)
        self._view.header().setSectionResizeMode(0, QHeaderView.ResizeMode.Stretch)
        self._view.header().setSectionResizeMode(
            QHeaderView.ResizeMode.ResizeToContents
        )

        container = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(self._view)
        container.setLayout(layout)
        self.setWidget(container)


class MethodsDock(QDockWidget):
    def __init__(self):
        super().__init__("Methods")
        self._init_ui()

    def _init_ui(self):
        self._model = MethodsModel()
        self._view = QTableView()
        self._view.setModel(self._model)
        self._view.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self._view.setAlternatingRowColors(True)
        self._view.horizontalHeader().setHidden(True)

        container = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(self._view)
        container.setLayout(layout)
        self.setWidget(container)


class RecorderDock(QDockWidget):
    def __init__(self):
        super().__init__("Recorder")


class TerminalDock(QDockWidget):
    def __init__(self):
        super().__init__("Terminal")


class ViewerWidget(QWidget):
    def __init__(self):
        super().__init__()


class MainWindow(QMainWindow):
    def __init__(self, client: Client):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        self.setWindowTitle("Aegis App")
        self.resize(800, 600)
        self.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)

        self._objects_dock = ObjectsDock()
        self._properties_dock = PropertiesDock()
        self._methods_dock = MethodsDock()
        self._terminal_dock = TerminalDock()
        self._recorder_dock = RecorderDock()

        self.setCentralWidget(ViewerWidget())
        self.addDockWidget(Qt.DockWidgetArea.RightDockWidgetArea, self._objects_dock)
        self.addDockWidget(Qt.DockWidgetArea.LeftDockWidgetArea, self._properties_dock)
        self.addDockWidget(Qt.DockWidgetArea.LeftDockWidgetArea, self._methods_dock)
        self.addDockWidget(Qt.DockWidgetArea.BottomDockWidgetArea, self._terminal_dock)
        self.addDockWidget(Qt.DockWidgetArea.BottomDockWidgetArea, self._recorder_dock)

        self.tabifyDockWidget(self._terminal_dock, self._recorder_dock)
