import enum

from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLineEdit,
    QPushButton,
    QTreeWidget,
    QTreeWidgetItem,
)
from PySide6.QtCore import Signal, Slot, Qt

from aegis import CommandClient, ClientException
from aegis_console.pages.page import Page


class Response:
    class Level(enum.Enum):
        Info = 1
        Error = 2

    def __init__(self, message: str, level: Level):
        self.message = message
        self.level = level


class ConsoleWidget(QWidget):
    command_entered = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._init_ui()

    def append(self, request: str, response: Response):
        request_color = Qt.GlobalColor.green
        response_color = Qt.GlobalColor.white

        if response.level == Response.Level.Error:
            request_color = Qt.GlobalColor.red

        request_item = QTreeWidgetItem([request])
        request_item.setForeground(0, request_color)

        if len(response.message) > 0:
            response_item = QTreeWidgetItem([response.message])
            response_item.setForeground(0, response_color)
            request_item.addChild(response_item)

        self._console_output.addTopLevelItem(request_item)

    def clear(self):
        self._console_output.clear()

    def _init_ui(self):
        self._console_output = QTreeWidget(self)
        self._console_output.setHeaderHidden(True)
        self._console_output.setStyleSheet("background-color: black; color: white;")
        self._console_output.setSelectionMode(QTreeWidget.SelectionMode.NoSelection)
        self._console_output.setFocusPolicy(Qt.FocusPolicy.NoFocus)

        self._main_layout = QVBoxLayout(self)
        self._main_layout.addWidget(self._console_output)

        self.setLayout(self._main_layout)


class ClientEditor(Page):
    detached = Signal()

    def __init__(self):
        super().__init__()
        self._init_ui()

    def activate_page(self, **kwargs):
        self._set_client(kwargs["client"])

    def deactivate_page(self):
        self._console_widget.clear()
        self._command_line.clear()

    def _init_ui(self):
        self._main_layout = QVBoxLayout(self)

        self._console_widget = ConsoleWidget(self)
        self._close_button = QPushButton("Close", self)

        self._command_line = QLineEdit(self)
        self._command_line.setStyleSheet("background-color: black; color: white;")
        self._command_line.setPlaceholderText("Enter command here...")

        self._main_layout.addWidget(self._console_widget)
        self._main_layout.addWidget(self._command_line)
        self._main_layout.addWidget(self._close_button)

        self._command_line.returnPressed.connect(self._handle_command_entered)
        self._close_button.clicked.connect(self._handle_close_pressed)

    def _set_client(self, client: CommandClient):
        assert client != None
        self._client = client
        self._client.disconnected.connect(self.detached)

    @Slot()
    def _handle_command_entered(self):
        request = self._command_line.text()

        try:
            self._client._marker_stub
            response = self._client.execute(request)
            self._console_widget.append(
                request, Response(str(response), Response.Level.Info)
            )
        except ClientException as e:
            self._console_widget.append(request, Response(str(e), Response.Level.Error))

        self._command_line.clear()

    @Slot()
    def _handle_close_pressed(self):
        self._client.close()
        self.detached.emit()
