from PySide6.QtWidgets import (
    QStackedWidget,
)

from aegis_client.pages.page import Page


class StackedPage(QStackedWidget):
    def __init__(self):
        super().__init__()

    def setCurrentWidget(self, page: Page, **kwargs):
        super().setCurrentWidget(page)
        page.setAsCurrent(**kwargs)
