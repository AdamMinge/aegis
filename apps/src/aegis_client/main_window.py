from PySide6.QtWidgets import QMainWindow, QWidget, QStackedWidget, QVBoxLayout
from PySide6.QtCore import Slot

from pages import (
    SelectAttachStrategyPage,
    AttachToProcessPage,
    ClientPage,
    RunAndAttachPage,
)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.__init_ui()
        self.__init_pages()

    def __init_ui(self):
        self.setWindowTitle("Aegis Simple Client")
        self.setGeometry(100, 100, 800, 600)

        self.__central_widget = QWidget()
        self.setCentralWidget(self.__central_widget)

        self.__stack = QStackedWidget()
        self.__layout = QVBoxLayout(self.__central_widget)
        self.__layout.addWidget(self.__stack)

        self.__stack.currentChanged.connect(self.adjustSize)

    def __init_pages(self):
        self.__select_attach_strategy_page = SelectAttachStrategyPage()
        self.__attach_to_process_page = AttachToProcessPage()
        self.__run_and_attach_page = RunAndAttachPage()
        self.__client_page = ClientPage()

        self.__stack.addWidget(self.__select_attach_strategy_page)
        self.__stack.addWidget(self.__attach_to_process_page)
        self.__stack.addWidget(self.__run_and_attach_page)
        self.__stack.addWidget(self.__client_page)

        self.__select_attach_strategy_page.selected.connect(
            self.__handle_selected_strategy
        )
        self.__attach_to_process_page.attached.connect(self.__handle_attached_process)
        self.__run_and_attach_page.attached.connect(self.__handle_attached_process)
        self.__client_page.detached.connect(self.__handle_detached_process)

        self.__stack.setCurrentWidget(self.__select_attach_strategy_page)

    @Slot(SelectAttachStrategyPage.Strategy)
    def __handle_selected_strategy(self, strategy: SelectAttachStrategyPage.Strategy):
        if strategy == SelectAttachStrategyPage.Strategy.AttachToProcess:
            self.__stack.setCurrentWidget(self.__attach_to_process_page)
        elif strategy == SelectAttachStrategyPage.Strategy.RunAndAttach:
            self.__stack.setCurrentWidget(self.__run_and_attach_page)

    @Slot(int)
    def __handle_attached_process(self, pid: int):
        self.__stack.setCurrentWidget(self.__client_page)

    @Slot()
    def __handle_detached_process(self):
        self.__stack.setCurrentWidget(self.__select_attach_strategy_page)
