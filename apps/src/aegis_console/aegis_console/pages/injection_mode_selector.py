import enum

from PySide6.QtWidgets import QHBoxLayout, QToolButton
from PySide6.QtCore import Signal, Slot, QSize
from PySide6.QtGui import QIcon, QPixmap

from aegis_console.pages.page import Page


class InjectionModeSelector(Page):
    class Mode(enum.Enum):
        AttachToProcess = 1
        StartNewProcess = 2

    selected_mode = Signal(Mode)

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        layout = QHBoxLayout(self)

        self.attach_to_process_button = QToolButton(self)
        self.attach_to_process_button.setIcon(
            QIcon(QPixmap(":/icons/attach_to_process.png"))
        )
        self.attach_to_process_button.setIconSize(QSize(128, 128))
        self.attach_to_process_button.setToolTip("Attach to process")

        self.start_new_process_button = QToolButton(self)
        self.start_new_process_button.setIcon(
            QIcon(QPixmap(":/icons/start_new_process.png"))
        )
        self.start_new_process_button.setIconSize(QSize(128, 128))
        self.start_new_process_button.setToolTip("Start new process")

        layout.addWidget(self.attach_to_process_button)
        layout.addWidget(self.start_new_process_button)

        self.attach_to_process_button.pressed.connect(
            self.__handle_attach_to_process_pressed
        )
        self.start_new_process_button.pressed.connect(
            self.__handle_start_new_process_pressed
        )

    @Slot()
    def __handle_attach_to_process_pressed(self):
        self.selected_mode.emit(self.Mode.AttachToProcess)

    @Slot()
    def __handle_start_new_process_pressed(self):
        self.selected_mode.emit(self.Mode.StartNewProcess)
