import sys

from PySide2.QtCore import QCoreApplication, Qt
from PySide2.QtWidgets import (QApplication)

from mainwindow import MainWindow

if __name__ == "__main__":
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    app = QApplication(sys.argv)
    widget = MainWindow()
    widget.show()

    sys.exit(app.exec_())
