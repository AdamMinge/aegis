import os

from PySide6.QtWidgets import (
    QHBoxLayout,
    QLineEdit,
    QPushButton,
    QFileDialog,
)
from PySide6.QtCore import Signal, Slot
from PySide6.QtNetwork import QHostAddress

from aegis import CommandClient, ClientException
from aegis_console import AEGIS_CLIENT_PORT, AEGIS_CLIENT_DLL
from aegis_console.pages.page import PageWithBack


class StartNewProcess(PageWithBack):
    attached = Signal(CommandClient)
    back_clicked = Signal()

    def _init_ui(self):
        super()._init_ui()

        self._application_path = QLineEdit(self)
        self._application_path.setPlaceholderText("Enter path to application...")

        self._browse_application = QPushButton("Browse")

        self._attach_button = QPushButton(self)
        self._attach_button.setText("Attach")
        self._attach_button.setToolTip("Attach to selected process")

        application_path_layout = QHBoxLayout()
        application_path_layout.addWidget(self._application_path)
        application_path_layout.addWidget(self._browse_application)

        attach_button_layout = QHBoxLayout()
        attach_button_layout.addStretch()
        attach_button_layout.addWidget(self._attach_button)

        self.layout().addLayout(application_path_layout)  # type: ignore
        self.layout().addStretch()  # type: ignore
        self.layout().addLayout(attach_button_layout)  # type: ignore

        self._application_path.textChanged.connect(self._handle_application_changed)
        self._browse_application.pressed.connect(self._handle_browse_pressed)
        self._attach_button.pressed.connect(self._handle_attach_pressed)

    def deactivate_page(self):
        self._clear()

    @Slot()
    def _clear(self):
        self._application_path.clear()
        self._handle_application_changed()

    @Slot()
    def _handle_application_changed(self):
        path = self._application_path.text()
        is_executable = os.path.isfile(path) and os.access(path, os.X_OK)
        self._attach_button.setEnabled(is_executable)

    @Slot()
    def _handle_browse_pressed(self):
        if os.name == "nt":
            file_filter = "Executable Files (*.exe);;All Files (*)"
        else:
            file_filter = "Executable Files (*);;All Files (*)"

        file_dialog = QFileDialog(self)
        file_dialog.setNameFilter(file_filter)
        file_path, _ = file_dialog.getOpenFileName(
            self, "Select Application", "", file_filter
        )
        if file_path:
            self._application_path.setText(file_path)

    @Slot()
    def _handle_attach_pressed(self):
        try:
            client = CommandClient.attach_to_new_process(
                QHostAddress(QHostAddress.SpecialAddress.LocalHost),
                AEGIS_CLIENT_PORT,
                self._application_path.text(),
                AEGIS_CLIENT_DLL,
            )

        except ClientException as e:
            return

        self.attached.emit(client)
