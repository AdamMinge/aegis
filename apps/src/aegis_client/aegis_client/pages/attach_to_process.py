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

from aegis import Client, ClientException
from aegis_client import AEGIS_CLIENT_PORT, AEGIS_CLIENT_DLL
from aegis_client.pages.page import PageWithBack


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
        self.__init_ui()

    def __init_ui(self):
        self.__model = QStandardItemModel(0, 3, self)
        self.__model.setHorizontalHeaderLabels(["Process", "ID", "User"])

        self.__proxy_model = ProcessTable.SortFilterProxyModel(self)
        self.__proxy_model.setSourceModel(self.__model)
        self.__proxy_model.setFilterCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.__proxy_model.setFilterKeyColumn(0)

        self.setModel(self.__proxy_model)
        self.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self.setSelectionMode(QTableView.SelectionMode.SingleSelection)
        self.setSortingEnabled(True)
        self.sortByColumn(ProcessTable.Columns.Name, Qt.SortOrder.AscendingOrder)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)
        self.horizontalHeader().setSectionsClickable(True)
        self.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Interactive)

        self.selectionModel().selectionChanged.connect(self.__handle_current_changed)

    def filter(self, filter):
        self.__proxy_model.setFilterWildcard(filter)

    def refresh(self):
        self.__model.removeRows(0, self.__model.rowCount())

        for proc in psutil.process_iter(["pid", "name", "username"]):
            try:
                name = proc.info["name"]
                pid = proc.info["pid"]
                username = proc.info["username"]
                self.__append_process(Process(name, pid, username))

            except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                continue

    def current(self) -> typing.Optional[Process]:
        indexes = self.selectionModel().selectedIndexes()
        if len(indexes) == 0:
            return None

        index = indexes[0]
        name = str(self.__model.item(index.row(), ProcessTable.Columns.Name).text())
        id = int(self.__model.item(index.row(), ProcessTable.Columns.ID).text())
        username = str(self.__model.item(index.row(), ProcessTable.Columns.User).text())

        return Process(name, id, username)

    def __append_process(self, process: Process):
        name = QStandardItem(process.name)
        id = QStandardItem(str(process.id))
        username = QStandardItem(process.user)

        name.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)
        id.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)
        username.setFlags(Qt.ItemFlag.ItemIsSelectable | Qt.ItemFlag.ItemIsEnabled)

        self.__model.appendRow([name, id, username])

    @Slot()
    def __handle_current_changed(self):
        self.current_changed.emit(self.current())


class AttachToProcess(PageWithBack):
    attached = Signal(Client)
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.__filter_processes = QLineEdit(self)
        self.__filter_processes.setPlaceholderText("Filter Processes")

        self.__process_table = ProcessTable()

        self.__refresh_button = QToolButton(self)
        self.__refresh_button.setText("Refresh")
        self.__refresh_button.setToolTip("Refresh process list")
        self.__refresh_button.setIcon(QIcon(QPixmap(":/icons/refresh.png")))
        self.__refresh_button.setIconSize(QSize(32, 32))

        self.__attach_button = QPushButton(self)
        self.__attach_button.setText("Attach")
        self.__attach_button.setToolTip("Attach to selected process")

        button_layout = QVBoxLayout()
        button_layout.addWidget(self.__refresh_button)
        button_layout.addWidget(self.__attach_button)

        attach_button_layout = QHBoxLayout()
        attach_button_layout.addStretch()
        attach_button_layout.addWidget(self.__attach_button)

        self.layout().addWidget(self.__filter_processes)
        self.layout().addWidget(self.__process_table)
        self.layout().addWidget(self.__refresh_button)
        self.layout().addLayout(attach_button_layout)  # type: ignore

        self.__filter_processes.textChanged.connect(self.__handle_filter_changed)
        self.__process_table.current_changed.connect(self.__handle_process_changed)
        self.__refresh_button.pressed.connect(self.__handle_refresh_pressed)
        self.__attach_button.pressed.connect(self.__handle_attach_pressed)

    def deactivate_page(self):
        self.__clear()

    @Slot()
    def __clear(self):
        self.__filter_processes.clear()
        self.__process_table.refresh()
        self.__handle_process_changed(None)

    @Slot()
    def __handle_refresh_pressed(self):
        self.__process_table.refresh()

    @Slot(Process, type(None))
    def __handle_process_changed(self, process: typing.Optional[Process]):
        self.__attach_button.setDisabled(process == None)

    @Slot()
    def __handle_filter_changed(self):
        filter = self.__filter_processes.text().lower()
        self.__process_table.filter(filter)

    @Slot()
    def __handle_attach_pressed(self):
        process = self.__process_table.current()
        assert process != None

        try:
            client = Client.attach_to_existing_process(
                QHostAddress(QHostAddress.SpecialAddress.LocalHost),
                AEGIS_CLIENT_PORT,
                process.id,
                AEGIS_CLIENT_DLL,
            )
        except ClientException as e:
            return

        self.attached.emit(client)
