import json

from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLineEdit,
    QPushButton,
    QTreeWidget,
    QTreeWidgetItem,
)
from PySide6.QtCore import Signal, Slot, Qt

from aegis import Client
from aegis_console.pages.page import Page


class ConsoleWidget(QWidget):
    command_entered = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.__init_ui()

    def __init_ui(self):
        self.__console_output = QTreeWidget(self)
        self.__console_output.setHeaderHidden(True)
        self.__console_output.setStyleSheet("background-color: black; color: white;")
        self.__console_output.setSelectionMode(QTreeWidget.SelectionMode.NoSelection)
        self.__console_output.setFocusPolicy(Qt.FocusPolicy.NoFocus)

        self.__main_layout = QVBoxLayout(self)
        self.__main_layout.addWidget(self.__console_output)

        self.setLayout(self.__main_layout)

    def clear(self):
        self.__console_output.clear()


class ClientEditor(Page):
    detached = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()
        self.__init_predefined_commands()

    def __init_ui(self):
        self.__main_layout = QVBoxLayout(self)

        self.__console_widget = ConsoleWidget(self)
        self.__close_button = QPushButton("Close", self)

        self.__command_line = QLineEdit(self)
        self.__command_line.setStyleSheet("background-color: black; color: white;")
        self.__command_line.setPlaceholderText("Enter command here...")

        self.__main_layout.addWidget(self.__console_widget)
        self.__main_layout.addWidget(self.__command_line)
        self.__main_layout.addWidget(self.__close_button)

        self.__command_line.returnPressed.connect(self.__handle_command_entered)
        self.__close_button.clicked.connect(self.__handle_close_pressed)

    def __init_predefined_commands(self):
        self.__predefined_commands = {}
        self.__predefined_commands["clear"] = self.__clear_console_command

    def activate_page(self, **kwargs):
        self.__set_client(kwargs["client"])

    def __set_client(self, client: Client):
        assert client != None
        self.__client = client
        self.__client.disconnected.connect(self.detached)

    def deactivate_page(self):
        self.__console_widget.clear()
        self.__command_line.clear()

    @Slot()
    def __clear_console_command(self):
        self.__console_widget.clear()

    @Slot()
    def __handle_special_command(self, command: str) -> bool:
        if command in self.__predefined_commands:
            self.__predefined_commands[command]()
            return True
        return False

    @Slot()
    def __handle_command_entered(self):
        # command = self.__command_line.text()
        # if not self.__handle_special_command(command):
        #   response = self.__client.send(command)
        #   self.__console_widget.append(command, response)

        self.__command_line.clear()

    @Slot()
    def __handle_close_pressed(self):
        self.__client.close()
        self.detached.emit()
