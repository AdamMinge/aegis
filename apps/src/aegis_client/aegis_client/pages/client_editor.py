from PySide6.QtCore import Signal

from aegis_client.pages.page import Page


class ClientEditor(Page):
    detached = Signal()

    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        pass

    def setAsCurrent(self, **kwargs):
        pass
