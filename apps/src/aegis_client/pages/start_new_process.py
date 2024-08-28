from PySide6.QtWidgets import (
    QWidget,
    QToolButton,
    QHBoxLayout,
    QVBoxLayout,
    QSpacerItem,
    QSizePolicy,
)
from PySide6.QtCore import Signal, Slot, QSize
from PySide6.QtGui import QIcon, QPixmap


class StartNewProcess(QWidget):
    attached = Signal(int)
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.back_button = QToolButton(self)
        self.back_button.setIcon(QIcon(QPixmap(":/icons/arrow_left.png")))
        self.back_button.setIconSize(QSize(32, 32))
        self.back_button.setToolTip("Go back")

        h_layout = QHBoxLayout()
        h_layout.addSpacerItem(
            QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)  # type: ignore
        )
        h_layout.addWidget(self.back_button)

        v_layout = QVBoxLayout()
        v_layout.addLayout(h_layout)
        v_layout.addStretch()

        self.setLayout(v_layout)

        self.back_button.pressed.connect(self.__handle_back_pressed)

    @Slot()
    def __handle_back_pressed(self):
        self.back_clicked.emit()
