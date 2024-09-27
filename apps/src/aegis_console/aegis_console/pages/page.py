from PySide6.QtWidgets import (
    QWidget,
    QToolButton,
    QHBoxLayout,
    QVBoxLayout,
    QFrame,
)
from PySide6.QtCore import Signal, Slot, QSize
from PySide6.QtGui import QIcon, QPixmap


class Page(QWidget):
    def __init__(self):
        super().__init__()

    def activate_page(self, **kwargs):
        pass

    def deactivate_page(self):
        pass


class PageWithBack(Page):
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        self._back_button = QToolButton(self)
        self._back_button.setIcon(QIcon(QPixmap(":/icons/arrow_left.png")))
        self._back_button.setIconSize(QSize(32, 32))
        self._back_button.setToolTip("Go back")

        self._line_separator = QFrame()
        self._line_separator.setFrameShape(QFrame.Shape.HLine)
        self._line_separator.setFrameShadow(QFrame.Shadow.Sunken)

        back_button_layout = QHBoxLayout()
        back_button_layout.addStretch()
        back_button_layout.addWidget(self._back_button)

        self._main_layout = QVBoxLayout()
        self._main_layout.addLayout(back_button_layout)
        self._main_layout.addWidget(self._line_separator)

        self.setLayout(self._main_layout)

        self._back_button.pressed.connect(self._handle_back_pressed)

    @Slot()
    def _handle_back_pressed(self):
        self.back_clicked.emit()
