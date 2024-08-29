import enum
import psutil
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

from .page_with_back import PageWithBack


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
        indexes = self.selectionModel().selectedIndexes()
        if len(indexes) == 0:
            self.current_changed.emit(None)
            return

        index = indexes[0]
        name = str(self.__model.item(index.row(), ProcessTable.Columns.Name).text())
        id = int(self.__model.item(index.row(), ProcessTable.Columns.ID).text())
        username = str(self.__model.item(index.row(), ProcessTable.Columns.User).text())

        self.current_changed.emit(Process(name, id, username))


class AttachToProcess(PageWithBack):
    attached = Signal(int)
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()
        self.__handle_refresh_pressed()
        self.__handle_process_changed(None)

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

    @Slot()
    def __handle_refresh_pressed(self):
        self.__process_table.refresh()

    @Slot(Process)
    def __handle_process_changed(self, process: Process):
        self.__attach_button.setDisabled(process == None)

    @Slot()
    def __handle_filter_changed(self):
        filter = self.__filter_processes.text().lower()
        self.__process_table.filter(filter)

    @Slot()
    def __handle_attach_pressed(self):
        pass
