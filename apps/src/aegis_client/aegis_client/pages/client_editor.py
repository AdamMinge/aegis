import enum

from PySide6.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QPushButton, QTextEdit
from PySide6.QtCore import Signal, Slot
from PySide6.QtGui import QColor, QTextCursor, QTextCharFormat, QFont

from aegis import Client
from aegis_client.pages.page import Page


class ConsoleWidget(QWidget):
    class Level(enum.Enum):
        Request = 0
        Response = 1
        Warning = 2
        Error = 3

    command_entered = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.__init_ui()

    def __init_ui(self):
        self.__console_output = QTextEdit(self)
        self.__console_output.setReadOnly(True)
        self.__console_output.setFont(QFont("Courier", 10))
        self.__console_output.setStyleSheet("background-color: black; color: white;")

        self.__command_line = QLineEdit(self)
        self.__command_line.setFont(QFont("Courier", 10))
        self.__command_line.setStyleSheet("background-color: black; color: white;")
        self.__command_line.setPlaceholderText("Enter command here...")

        self.__main_layout = QVBoxLayout(self)
        self.__main_layout.addWidget(self.__console_output)
        self.__main_layout.addWidget(self.__command_line)

        self.__command_line.returnPressed.connect(self.__handle_command_entered)

        self.setLayout(self.__main_layout)

    def write(self, message: str, level: Level):
        if level == ConsoleWidget.Level.Error:
            color = QColor("red")
        elif level == ConsoleWidget.Level.Warning:
            color = QColor("yellow")
        elif level == ConsoleWidget.Level.Response:
            color = QColor("green")
        else:
            color = QColor("white")

        fmt = QTextCharFormat()
        fmt.setForeground(color)
        cursor = self.__console_output.textCursor()
        cursor.movePosition(QTextCursor.MoveOperation.End)
        cursor.insertText(message + "\n", fmt)
        self.__console_output.ensureCursorVisible()

    def clear(self):
        self.__console_output.clear()
        self.__command_line.clear()

    @Slot()
    def __handle_command_entered(self):
        command = self.__command_line.text()
        if command:
            self.command_entered.emit(command)
            self.__command_line.clear()


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

        self.__main_layout.addWidget(self.__console_widget)
        self.__main_layout.addWidget(self.__close_button)

        self.__console_widget.command_entered.connect(self.__handle_execute_command)
        self.__close_button.clicked.connect(self.__handle_close)

    def __init_predefined_commands(self):
        self.__predefined_commands = {}
        self.__predefined_commands["clear"] = self.__clear_console_command

    def activate_page(self, **kwargs):
        self.__set_client(kwargs["client"])

    def deactivate_page(self):
        self.__clear_console_command()

    @Slot()
    def __set_client(self, client: Client):
        self.__client = client
        self.__client.readyRead.connect(self.__handle_server_response)
        self.__client.disconnected.connect(self.__handle_close)

    @Slot()
    def __clear_console_command(self):
        self.__console_widget.clear()

    @Slot()
    def __handle_server_response(self):
        response = self.__client.readAll().data().decode("utf-8")  # type: ignore
        self.__console_widget.write(response, ConsoleWidget.Level.Response)

    @Slot()
    def __handle_special_command(self, command: str) -> bool:
        if command in self.__predefined_commands:
            self.__predefined_commands[command]()
            return True
        return False

    @Slot()
    def __handle_execute_command(self, command: str):
        if self.__handle_special_command(command):
            return

        self.__console_widget.write(command, ConsoleWidget.Level.Request)
        self.__client.write(command.encode("utf-8"))

    @Slot()
    def __handle_close(self):
        self.__client.close()
        self.detached.emit()
