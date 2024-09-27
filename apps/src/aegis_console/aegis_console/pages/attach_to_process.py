import enum
import psutil
import typing
import dataclasses

from PySide6.QtWidgets import (
    QToolButton,
    QHBoxLayout,
    QVBoxLayout,
    QLineEdit,
    QHeaderView,
    QTableView,
    QPushButton,
)
from PySide6.QtCore import Signal, Slot, QSize, QSortFilterProxyModel, Qt, QModelIndex
from PySide6.QtGui import QIcon, QPixmap, QStandardItemModel, QStandardItem
from PySide6.QtNetwork import QHostAddress

from aegis import CommandClient, ClientException
from aegis_console import AEGIS_CLIENT_PORT, AEGIS_CLIENT_DLL
from aegis_console.pages.page import PageWithBack


@dataclasses.dataclass
class Process:
    name: str
    id: int
    user: str


class ProcessTable(QTableView):
    class Columns(enum.IntEnum):
        Name = 0
        ID = 1
        User = 2

    class SortFilterProxyModel(QSortFilterProxyModel):
        def lessThan(self, left: QModelIndex, right: QModelIndex) -> bool:
            column = left.column()

            if column == ProcessTable.Columns.ID:
                left_value = int(left.data())
                right_value = int(right.data())
                return left_value < right_value
            else:
                return super().lessThan(left, right)

    current_changed = Signal(Process)

    def __init__(self):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        self._model = QStandardItemModel(0, 3, self)
        self._model.setHorizontalHeaderLabels(["Process", "ID", "User"])

        self._proxy_model = ProcessTable.SortFilterProxyModel(self)
        self._proxy_model.setSourceModel(self._model)
        self._proxy_model.setFilterCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self._proxy_model.setFilterKeyColumn(0)

        self.setModel(self._proxy_model)
        self.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self.setSelectionMode(QTableView.SelectionMode.SingleSelection)
        self.setSortingEnabled(True)
        self.sortByColumn(ProcessTable.Columns.Name, Qt.SortOrder.AscendingOrder)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)
        self.horizontalHeader().setSectionsClickable(True)
        self.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Interactive)

        self.selectionModel().selectionChanged.connect(self._handle_current_changed)

    def filter(self, filter):
        self._proxy_model.setFilterWildcard(filter)

    def refresh(self):
        self._model.removeRows(0, self._model.rowCount())

        for proc in psutil.process_iter(["pid", "name", "username"]):
            try:
                name = proc.info["name"]
                pid = proc.info["pid"]
                username = proc.info["username"]
                self._append_process(Process(name, pid, username))

            except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                continue

    def current(self) -> typing.Optional[Process]:
        indexes = self.selectionModel().selectedIndexes()
        if len(indexes) == 0:
            return None

        index = indexes[0]
        name = str(self._model.item(index.row(), ProcessTable.Columns.Name).text())
        id = int(self._model.item(index.row(), ProcessTable.Columns.ID).text())
        username = str(self._model.item(index.row(), ProcessTable.Columns.User).text())

        return Process(name, id, username)

    def _append_process(self, process: Process):
        name = QStandardItem(process.name)
        id = QStandardItem(str(process.id))
        username = QStandardItem(process.user)

        name.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)
        id.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)
        username.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)

        self._model.appendRow([name, id, username])

    @Slot()
    def _handle_current_changed(self):
        self.current_changed.emit(self.current())


class AttachToProcess(PageWithBack):
    attached = Signal(CommandClient)
    back_clicked = Signal()

    def _init_ui(self):
        super()._init_ui()

        self._filter_processes = QLineEdit(self)
        self._filter_processes.setPlaceholderText("Filter Processes")

        self._process_table = ProcessTable()

        self._refresh_button = QToolButton(self)
        self._refresh_button.setText("Refresh")
        self._refresh_button.setToolTip("Refresh process list")
        self._refresh_button.setIcon(QIcon(QPixmap(":/icons/refresh.png")))
        self._refresh_button.setIconSize(QSize(32, 32))

        self._attach_button = QPushButton(self)
        self._attach_button.setText("Attach")
        self._attach_button.setToolTip("Attach to selected process")

        button_layout = QVBoxLayout()
        button_layout.addWidget(self._refresh_button)
        button_layout.addWidget(self._attach_button)

        attach_button_layout = QHBoxLayout()
        attach_button_layout.addStretch()
        attach_button_layout.addWidget(self._attach_button)

        self.layout().addWidget(self._filter_processes)  # type: ignore
        self.layout().addWidget(self._process_table)  # type: ignore
        self.layout().addWidget(self._refresh_button)  # type: ignore
        self.layout().addLayout(attach_button_layout)  # type: ignore

        self._filter_processes.textChanged.connect(self._handle_filter_changed)
        self._process_table.current_changed.connect(self._handle_process_changed)
        self._refresh_button.pressed.connect(self._handle_refresh_pressed)
        self._attach_button.pressed.connect(self._handle_attach_pressed)

    def deactivate_page(self):
        self._clear()

    @Slot()
    def _clear(self):
        self._filter_processes.clear()
        self._process_table.refresh()
        self._handle_process_changed(None)

    @Slot()
    def _handle_refresh_pressed(self):
        self._process_table.refresh()

    @Slot(Process, type(None))
    def _handle_process_changed(self, process: typing.Optional[Process]):
        self._attach_button.setDisabled(process == None)

    @Slot()
    def _handle_filter_changed(self):
        filter = self._filter_processes.text().lower()
        self._process_table.filter(filter)

    @Slot()
    def _handle_attach_pressed(self):
        process = self._process_table.current()
        assert process != None

        try:
            client = CommandClient.attach_to_existing_process(
                QHostAddress(QHostAddress.SpecialAddress.LocalHost),
                AEGIS_CLIENT_PORT,
                process.id,
                AEGIS_CLIENT_DLL,
            )

        except ClientException as e:
            return

        self.attached.emit(client)
