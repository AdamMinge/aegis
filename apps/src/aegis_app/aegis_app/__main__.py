import sys

from aegis_app import Application, AttachWizard, MainWindow, rcc


def main():
    app = Application(sys.argv)

    attach_wizard = AttachWizard()
    client = attach_wizard.attach()
    if not client:
        sys.exit(app.exit())
        return

    window = MainWindow(client)
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
