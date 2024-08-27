import sys
import argparse


def open_simple_app():
    from simple import Application, MainWindow

    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


def open_extended_app():
    from extended import Application, MainWindow

    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


def main():
    parser = argparse.ArgumentParser(
        description="Run Aegis Client with different modes."
    )

    parser.add_argument(
        "--mode",
        choices=["simple", "extended"],
        default="simple",
        help="Select the mode of the application.",
    )
    args = parser.parse_args()

    if args.mode == "simple":
        open_simple_app()
    elif args.mode == "extended":
        open_extended_app()


if __name__ == "__main__":
    main()
