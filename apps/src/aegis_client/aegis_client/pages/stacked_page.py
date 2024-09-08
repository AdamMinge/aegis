from PySide6.QtWidgets import (
    QStackedWidget,
)

from aegis_client.pages.page import Page


class StackedPage(QStackedWidget):
    def __init__(self):
        super().__init__()

    def setCurrentWidget(self, page: Page, **kwargs):
        current_page: Page = super().currentWidget()  # type: ignore
        if current_page:
            current_page.deactivate_page()

        super().setCurrentWidget(page)

        if page:
            page.activate_page(**kwargs)
