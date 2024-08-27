import enum

from PySide6.QtWidgets import QWidget, QHBoxLayout, QToolButton
from PySide6.QtCore import Signal, Slot, QSize
from PySide6.QtGui import QIcon, QPixmap


class SelectAttachStrategyPage(QWidget):
    class Strategy(enum.Enum):
        AttachToProcess = 1
        RunAndAttach = 2

    selected = Signal(Strategy)

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
        self.attach_to_process_button.setToolTip("Attach to Process")

        self.run_and_attach_button = QToolButton(self)
        self.run_and_attach_button.setIcon(QIcon(QPixmap(":/icons/run_and_attach.png")))
        self.run_and_attach_button.setIconSize(QSize(128, 128))
        self.run_and_attach_button.setToolTip("Run and Attach")

        layout.addWidget(self.attach_to_process_button)
        layout.addWidget(self.run_and_attach_button)

        self.attach_to_process_button.pressed.connect(
            self.__handle_attach_to_process_pressed
        )
        self.run_and_attach_button.pressed.connect(self.__handle_run_and_attach_pressed)

    @Slot()
    def __handle_attach_to_process_pressed(self):
        self.selected.emit(self.Strategy.AttachToProcess)

    @Slot()
    def __handle_run_and_attach_pressed(self):
        self.selected.emit(self.Strategy.RunAndAttach)
