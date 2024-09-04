import os

from PySide6.QtWidgets import (
    QHBoxLayout,
    QLineEdit,
    QPushButton,
    QFileDialog,
)
from PySide6.QtCore import Signal, Slot
from PySide6.QtNetwork import QHostAddress

from aegis import Client, ClientException
from aegis_client import AEGIS_CLIENT_PORT, AEGIS_CLIENT_DLL
from aegis_client.pages.page import PageWithBack


class StartNewProcess(PageWithBack):
    attached = Signal(Client)
    back_clicked = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.__application_path = QLineEdit(self)
        self.__application_path.setPlaceholderText("Enter path to application...")

        self.__browse_application = QPushButton("Browse")

        self.__attach_button = QPushButton(self)
        self.__attach_button.setText("Attach")
        self.__attach_button.setToolTip("Attach to selected process")

        application_path_layout = QHBoxLayout()
        application_path_layout.addWidget(self.__application_path)
        application_path_layout.addWidget(self.__browse_application)

        attach_button_layout = QHBoxLayout()
        attach_button_layout.addStretch()
        attach_button_layout.addWidget(self.__attach_button)

        self.layout().addLayout(application_path_layout)  # type: ignore
        self.layout().addStretch()  # type: ignore
        self.layout().addLayout(attach_button_layout)  # type: ignore

        self.__application_path.textChanged.connect(self.__handle_application_changed)
        self.__browse_application.pressed.connect(self.__handle_browse_pressed)
        self.__attach_button.pressed.connect(self.__handle_attach_pressed)

    def setAsCurrent(self, **kwargs):
        self.__application_path.clear()
        self.__handle_application_changed()

    @Slot()
    def __handle_application_changed(self):
        path = self.__application_path.text()
        is_executable = os.path.isfile(path) and os.access(path, os.X_OK)
        self.__attach_button.setEnabled(is_executable)

    @Slot()
    def __handle_browse_pressed(self):
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
            self.__application_path.setText(file_path)

    @Slot()
    def __handle_attach_pressed(self):
        try:
            client = Client.attach_to_new_process(
                QHostAddress(QHostAddress.SpecialAddress.LocalHost),
                AEGIS_CLIENT_PORT,
                self.__application_path.text(),
                AEGIS_CLIENT_DLL,
            )
        except ClientException as e:
            return

        self.attached.emit(client)
