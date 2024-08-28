import enum
import psutil
import dataclasses

from PySide6.QtWidgets import (
    QWidget,
    QToolButton,
    QHBoxLayout,
    QVBoxLayout,
    QLineEdit,
    QHeaderView,
    QTableView,
)
from PySide6.QtCore import (
    Signal,
    Slot,
    QSize,
    QSortFilterProxyModel,
    Qt,
)
from PySide6.QtGui import QIcon, QPixmap, QStandardItemModel, QStandardItem


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

    current_changed = Signal(Process)

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.__model = QStandardItemModel(0, 3, self)
        self.__model.setHorizontalHeaderLabels(["Process", "ID", "User"])

        self.__proxy_model = QSortFilterProxyModel(self)
        self.__proxy_model.setSourceModel(self.__model)
        self.__proxy_model.setFilterCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.__proxy_model.setFilterKeyColumn(0)

        self.setModel(self.__proxy_model)
        self.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        self.horizontalHeader().setStretchLastSection(True)
        self.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)

        self.selectionModel().currentRowChanged.connect(self.__handle_current_changed)

    def filter(self, filter):
        self.__proxy_model.setFilterWildcard(filter)

    def refresh(self):
        for proc in psutil.process_iter(["pid", "name", "username"]):
            try:
                name = QStandardItem(proc.info["name"])
                pid = QStandardItem(str(proc.info["pid"]))
                username = QStandardItem(proc.info["username"])

                self.__model.appendRow([name, pid, username])

            except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                continue

    @Slot()
    def __handle_current_changed(self):
        index = self.currentIndex()
        if not index.isValid():
            self.current_changed.emit(None)
            return

        name = str(self.__model.item(index.row(), ProcessTable.Columns.Name).text())
        id = int(self.__model.item(index.row(), ProcessTable.Columns.ID).text())
        username = str(self.__model.item(index.row(), ProcessTable.Columns.User).text())

        self.current_changed.emit(Process(name, id, username))


class AttachToProcess(QWidget):
    attached = Signal(int)
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()
        self.__handle_refresh_pressed()
        self.__handle_process_changed(None)

    def __init_ui(self):
        self.back_button = QToolButton(self)
        self.back_button.setIcon(QIcon(QPixmap(":/icons/arrow_left.png")))
        self.back_button.setIconSize(QSize(32, 32))
        self.back_button.setToolTip("Go back")

        self.filter_processes = QLineEdit(self)
        self.filter_processes.setPlaceholderText("Filter Processes")

        self.process_table = ProcessTable()

        self.refresh_button = QToolButton(self)
        self.refresh_button.setText("Refresh")
        self.refresh_button.setToolTip("Refresh process list")
        self.refresh_button.setIcon(QIcon(QPixmap(":/icons/refresh.png")))
        self.refresh_button.setIconSize(QSize(32, 32))

        self.attach_button = QToolButton(self)
        self.attach_button.setText("Attach")
        self.attach_button.setToolTip("Attach to selected process")

        back_button_layout = QHBoxLayout()
        back_button_layout.addStretch()
        back_button_layout.addWidget(self.back_button)

        button_layout = QVBoxLayout()
        button_layout.addWidget(self.refresh_button)
        button_layout.addWidget(self.attach_button)

        attach_button_layout = QHBoxLayout()
        attach_button_layout.addStretch()
        attach_button_layout.addWidget(self.attach_button)

        main_layout = QVBoxLayout()
        main_layout.addLayout(back_button_layout)
        main_layout.addWidget(self.filter_processes)
        main_layout.addWidget(self.process_table)
        main_layout.addWidget(self.refresh_button)
        main_layout.addLayout(attach_button_layout)

        self.setLayout(main_layout)

        self.back_button.pressed.connect(self.__handle_back_pressed)
        self.filter_processes.textChanged.connect(self.__handle_filter_changed)
        self.process_table.current_changed.connect(self.__handle_process_changed)
        self.refresh_button.pressed.connect(self.__handle_refresh_pressed)
        self.attach_button.pressed.connect(self.__handle_attach_pressed)

    @Slot()
    def __handle_back_pressed(self):
        self.back_clicked.emit()

    @Slot()
    def __handle_refresh_pressed(self):
        self.process_table.refresh()

    @Slot(Process)
    def __handle_process_changed(self, process: Process):
        self.attach_button.setDisabled(process == None)

    @Slot()
    def __handle_filter_changed(self):
        filter = self.filter_processes.text().lower()
        self.process_table.filter(filter)

    @Slot()
    def __handle_attach_pressed(self):
        pass
