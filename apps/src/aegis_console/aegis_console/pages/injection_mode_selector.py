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
        self._init_ui()

    def _init_ui(self):
        layout = QHBoxLayout(self)

        self._attach_to_process_button = QToolButton(self)
        self._attach_to_process_button.setIcon(
            QIcon(QPixmap(":/icons/attach_to_process.png"))
        )
        self._attach_to_process_button.setIconSize(QSize(128, 128))
        self._attach_to_process_button.setToolTip("Attach to process")

        self._start_new_process_button = QToolButton(self)
        self._start_new_process_button.setIcon(
            QIcon(QPixmap(":/icons/start_new_process.png"))
        )
        self._start_new_process_button.setIconSize(QSize(128, 128))
        self._start_new_process_button.setToolTip("Start new process")

        layout.addWidget(self._attach_to_process_button)
        layout.addWidget(self._start_new_process_button)

        self._attach_to_process_button.pressed.connect(
            self._handle_attach_to_process_pressed
        )
        self._start_new_process_button.pressed.connect(
            self._handle_start_new_process_pressed
        )

    @Slot()
    def _handle_attach_to_process_pressed(self):
        self.selected_mode.emit(self.Mode.AttachToProcess)

    @Slot()
    def _handle_start_new_process_pressed(self):
        self.selected_mode.emit(self.Mode.StartNewProcess)
