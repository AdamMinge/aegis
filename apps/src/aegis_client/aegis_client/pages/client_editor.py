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

from aegis import Client, ClientResponse
from aegis_client.pages.page import Page


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

    def append(self, request: str, response: ClientResponse):
        response_color = Qt.GlobalColor.white
        if response.level == ClientResponse.Level.Error:
            request_color = Qt.GlobalColor.red
        elif response.level == ClientResponse.Level.Warning:
            request_color = Qt.GlobalColor.yellow
        else:
            request_color = Qt.GlobalColor.green

        request_item = QTreeWidgetItem([request])
        request_item.setForeground(0, request_color)

        if response.body:
            json_body = json.dumps(response.body, indent=4, sort_keys=True)

            response_item = QTreeWidgetItem([json_body])
            response_item.setForeground(0, response_color)
            request_item.addChild(response_item)

        self.__console_output.addTopLevelItem(request_item)

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

    def deactivate_page(self):
        self.__console_widget.clear()
        self.__command_line.clear()

    @Slot()
    def __set_client(self, client: Client):
        self.__client = client
        self.__client.closed.connect(self.__handle_closed)

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
        command = self.__command_line.text()
        if not self.__handle_special_command(command):
            response = self.__client.send(command)
            self.__console_widget.append(command, response)

        self.__command_line.clear()

    @Slot()
    def __handle_closed(self):
        self.detached.emit()

    @Slot()
    def __handle_close_pressed(self):
        self.__client.close()
