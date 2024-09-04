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

    def setAsCurrent(self, **kwargs):
        pass


class PageWithBack(Page):
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.__back_button = QToolButton(self)
        self.__back_button.setIcon(QIcon(QPixmap(":/icons/arrow_left.png")))
        self.__back_button.setIconSize(QSize(32, 32))
        self.__back_button.setToolTip("Go back")

        self.__line_separator = QFrame()
        self.__line_separator.setFrameShape(QFrame.Shape.HLine)
        self.__line_separator.setFrameShadow(QFrame.Shadow.Sunken)

        back_button_layout = QHBoxLayout()
        back_button_layout.addStretch()
        back_button_layout.addWidget(self.__back_button)

        self.__main_layout = QVBoxLayout()
        self.__main_layout.addLayout(back_button_layout)
        self.__main_layout.addWidget(self.__line_separator)

        self.setLayout(self.__main_layout)

        self.__back_button.pressed.connect(self.__handle_back_pressed)

    @Slot()
    def __handle_back_pressed(self):
        self.back_clicked.emit()
