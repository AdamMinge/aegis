from PySide6.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QSizePolicy,
)
from PySide6.QtCore import Slot

from aegis import Client
from aegis_console.pages import (
    StackedPage,
    AttachToProcess,
    ClientEditor,
    InjectionModeSelector,
    StartNewProcess,
)


class Application(QApplication):
    def __init__(self, argv):
        super().__init__(argv)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.__init_ui()
        self.__init_pages()

    def __init_ui(self):
        self.setWindowTitle("Aegis Client")
        self.resize(800, 600)
        self.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)

        self.__central_widget = QWidget()
        self.setCentralWidget(self.__central_widget)

        self.__stack = StackedPage()
        self.__layout = QVBoxLayout(self.__central_widget)
        self.__layout.addWidget(self.__stack)

        self.__stack.currentChanged.connect(self.adjustSize)

    def __init_pages(self):
        self.__injection_mode_selector_page = InjectionModeSelector()
        self.__attach_to_process_page = AttachToProcess()
        self.__start_new_process_page = StartNewProcess()
        self.__client_editor_page = ClientEditor()

        self.__stack.addWidget(self.__injection_mode_selector_page)
        self.__stack.addWidget(self.__attach_to_process_page)
        self.__stack.addWidget(self.__start_new_process_page)
        self.__stack.addWidget(self.__client_editor_page)

        self.__injection_mode_selector_page.selected_mode.connect(
            self.__handle_selected_mode
        )
        self.__attach_to_process_page.attached.connect(self.__handle_attached_process)
        self.__attach_to_process_page.back_clicked.connect(
            self.__handle_detached_process
        )
        self.__start_new_process_page.attached.connect(self.__handle_attached_process)
        self.__start_new_process_page.back_clicked.connect(
            self.__handle_detached_process
        )
        self.__client_editor_page.detached.connect(self.__handle_detached_process)

        self.__stack.setCurrentWidget(self.__injection_mode_selector_page)

    @Slot(InjectionModeSelector.Mode)
    def __handle_selected_mode(self, strategy: InjectionModeSelector.Mode):
        if strategy == InjectionModeSelector.Mode.AttachToProcess:
            self.__stack.setCurrentWidget(self.__attach_to_process_page)
        elif strategy == InjectionModeSelector.Mode.StartNewProcess:
            self.__stack.setCurrentWidget(self.__start_new_process_page)

    @Slot(Client)
    def __handle_attached_process(self, client: Client):
        self.__stack.setCurrentWidget(self.__client_editor_page, client=client)

    @Slot()
    def __handle_detached_process(self):
        self.__stack.setCurrentWidget(self.__injection_mode_selector_page)
