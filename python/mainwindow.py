from PySide2.QtCore import Qt, QDateTime, QRegExp, Slot
from PySide2.QtWidgets import QMainWindow

from sketchwidget import SketchWidget
from switchbutton import SwitchButton
from ui_mainwindow import Ui_MainWindow


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.mdi_area = self.ui.mdiArea
        self.mode_switch_button = SwitchButton()
        self.mode_switch_button.setParent(self.ui.switchContainer)
        self.current_sketch_widget = None
        self.sketch_widget_name_map = {}

        def clear_sketch_widget():
            if self.current_sketch_widget is not None:
                self.current_sketch_widget.clear_scene()
                self.current_sketch_widget.update()

        self.ui.clearSceneButton.clicked.connect(clear_sketch_widget)

        def new_sketch_widget():
            self.new_sketch_widget()

        self.ui.newSketchButton.clicked.connect(new_sketch_widget)
        self.new_sketch_widget()
        self.showMaximized()

    def resizeEvent(self, event):
        super(MainWindow, self).resizeEvent(event)
        self.mode_switch_button.resize(self.ui.switchContainer.size())

    @Slot()
    def syncCurrentSketchWidget(self, focusIn):
        if focusIn:
            self.current_sketch_widget = self.sender()
            print(self.current_sketch_widget)

    def new_sketch_widget(self, doc_name=''):
        sketch_widget = SketchWidget()
        self.mode_switch_button.clicked.connect(sketch_widget.switch_mode)
        self.mdi_area.addSubWindow(sketch_widget, Qt.FramelessWindowHint)
        sketch_widget.showMaximized()
        sketch_widget.switch_mode(self.mode_switch_button.isChecked())

        def removeName():
            del self.sketch_widget_name_map[sketch_widget]

        sketch_widget.closed.connect(removeName)

        # 一旦画图区域得到焦点，把当前绘图区指针指向这个窗体
        sketch_widget.focus_state_changed.connect(self.syncCurrentSketchWidget)
        if doc_name is "":
            tmp_doc_name = "新文档-" + QDateTime.currentDateTime().toString(Qt.DateFormat.ISODate).replace(
                QRegExp("\\s|/|:").__str__(), "-")
            sketch_widget.setWindowTitle(tmp_doc_name)
            self.sketch_widget_name_map[sketch_widget] = tmp_doc_name
        else:
            sketch_widget.setWindowTitle(doc_name)
            self.sketch_widget_name_map[sketch_widget] = doc_name

        self.current_sketch_widget = sketch_widget
