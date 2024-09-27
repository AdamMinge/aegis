from PySide6.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QSizePolicy,
)
from PySide6.QtCore import Slot

from aegis import CommandClient
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
        self._init_ui()
        self._init_pages()

    def _init_ui(self):
        self.setWindowTitle("Aegis Client")
        self.resize(800, 600)
        self.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)

        self._central_widget = QWidget()
        self.setCentralWidget(self._central_widget)

        self._stack = StackedPage()
        self._layout = QVBoxLayout(self._central_widget)
        self._layout.addWidget(self._stack)

        self._stack.currentChanged.connect(self.adjustSize)

    def _init_pages(self):
        self._injection_mode_selector_page = InjectionModeSelector()
        self._attach_to_process_page = AttachToProcess()
        self._start_new_process_page = StartNewProcess()
        self._client_editor_page = ClientEditor()

        self._stack.addWidget(self._injection_mode_selector_page)
        self._stack.addWidget(self._attach_to_process_page)
        self._stack.addWidget(self._start_new_process_page)
        self._stack.addWidget(self._client_editor_page)

        self._injection_mode_selector_page.selected_mode.connect(
            self._handle_selected_mode
        )
        self._attach_to_process_page.attached.connect(self._handle_attached_process)
        self._attach_to_process_page.back_clicked.connect(self._handle_detached_process)
        self._start_new_process_page.attached.connect(self._handle_attached_process)
        self._start_new_process_page.back_clicked.connect(self._handle_detached_process)
        self._client_editor_page.detached.connect(self._handle_detached_process)

        self._stack.setCurrentWidget(self._injection_mode_selector_page)

    @Slot(InjectionModeSelector.Mode)
    def _handle_selected_mode(self, strategy: InjectionModeSelector.Mode):
        if strategy == InjectionModeSelector.Mode.AttachToProcess:
            self._stack.setCurrentWidget(self._attach_to_process_page)
        elif strategy == InjectionModeSelector.Mode.StartNewProcess:
            self._stack.setCurrentWidget(self._start_new_process_page)

    @Slot(CommandClient)
    def _handle_attached_process(self, client: CommandClient):
        self._stack.setCurrentWidget(self._client_editor_page, client=client)

    @Slot()
    def _handle_detached_process(self):
        self._stack.setCurrentWidget(self._injection_mode_selector_page)
