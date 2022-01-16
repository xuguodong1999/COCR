from PySide2.QtCore import Qt, Signal, QPoint, QRect
from PySide2.QtGui import QColor, QPixmap, QPainter, QPen
from PySide2.QtWidgets import QWidget, QGraphicsItem, QGraphicsView,QLabel

from sketchscene import SketchScene
from sketchview import SketchView


class SketchWidget(QWidget):
    def __init__(self):
        super(SketchWidget, self).__init__()

        self.pen_color = QColor(Qt.black)
        self.pen_width = 5
        self.last_pos = QPoint()
        self.buf_pixmap = QPixmap()
        self.painter = QPainter()

        self.setFocusPolicy(Qt.StrongFocus)
        self.view = SketchView()
        self.view.setParent(self)
        self.view.setRenderHint(QPainter.Antialiasing)
        self.view.setCacheMode(QGraphicsView.CacheBackground)
        self.view.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.view.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.view.setRubberBandSelectionMode(
            Qt.ItemSelectionMode.ContainsItemBoundingRect)
        self.current_scene = SketchScene()
        self.view.setScene(self.current_scene)
        self.view.hide()

    def switch_mode(self, _show_sketch_view):
        if _show_sketch_view:
            self.show_sketch_view()
        else:
            self.show_scribble_area()

    @property
    def get_pen_color(self):
        return self.pen_color

    @property
    def get_pen_width(self):
        return self.pen_width

    @property
    def set_pen_color(self, penColor):
        self.pen_color = penColor

    @property
    def set_pen_width(self, penWidth):
        self.pen_width = penWidth

    pen_color_changed = Signal()
    pen_width_changed = Signal()
    focus_state_changed = Signal(bool)
    closed = Signal()
    pen_color_changed = Signal()
    pen_width_changed = Signal()
    focus_state_changed = Signal(bool)

    def clear_scene(self):
        if self.current_scene is not None:
            self.current_scene.clear_all()
            self.current_scene.add_back_ground(self.size())
            self.sync_sketch_view()

    def mousePressEvent(self, event):
        super(SketchWidget, self).mousePressEvent(event)
        if not self.view.isHidden():
            return
        self.last_pos = event.pos()
        self.painter.begin(self.buf_pixmap)
        self.painter.setPen(QPen(Qt.black, self.pen_width))
        self.painter.setRenderHint(QPainter.Antialiasing)
        self.painter.drawLine(self.last_pos, self.last_pos)
        self.update_rect(self.last_pos, self.last_pos)
        self.current_scene.addPathMoveTo(self.last_pos)
        self.current_scene.addPathLineTo(self.last_pos)

    def mouseMoveEvent(self, event):
        super(SketchWidget, self).mouseMoveEvent(event)
        if not self.view.isHidden():
            return
        current_pos = event.pos()
        self.painter.drawLine(self.last_pos, current_pos)
        self.update_rect(current_pos, self.last_pos)
        self.last_pos = current_pos
        self.current_scene.addPathLineTo(self.last_pos)

    def mouseReleaseEvent(self, event):
        super(SketchWidget, self).mouseReleaseEvent(event)
        if not self.view.isHidden():
            return
        item = self.current_scene.commitPath()
        item.setFlags(QGraphicsItem.ItemIsMovable |
                      QGraphicsItem.ItemIsFocusable | QGraphicsItem.ItemIsSelectable)
        item.setPen(QPen(Qt.black, self.pen_width))
        self.current_scene.clearPath()
        self.painter.end()

    def paintEvent(self, event):
        rect = event.rect()
        buf_painter = QPainter(self)
        buf_painter.setRenderHint(QPainter.Antialiasing)
        buf_painter.drawPixmap(rect, self.buf_pixmap, rect)

    def resizeEvent(self, event):
        super(SketchWidget, self).resizeEvent(event)
        self.view.resize(self.size())
        self.current_scene.setSceneRect(0, 0, self.width(), self.height())
        self.current_scene.add_back_ground(self.size())
        self.sync_sketch_view()

    def focusInEvent(self, event):
        super(SketchWidget, self).focusInEvent(event)
        self.focus_state_changed.emit(True)

    def closeEvent(self, event):
        super(SketchWidget, self).closeEvent(event)
        self.closed.emit()

    def sync_sketch_view(self):
        self.buf_pixmap = QPixmap(self.size())
        self.buf_pixmap.fill(Qt.white)
        if self.buf_pixmap.width() == 0 or self.buf_pixmap.height() == 0:
            return
        print('1:',self.size())
        self.painter.begin(self.buf_pixmap)
        self.view.render(self.painter, self.rect(), self.rect())
        self.painter.end()
        print('hello')
        # self.label=QLabel()
        # self.label.resize(self.buf_pixmap.size())
        # self.label.setPixmap(self.buf_pixmap)
        # self.label.show()

    def update_rect(self, p1, p2):
        if p2.x() < p1.x():
            xmin = p2.x()
            xmax = p1.x()
        else:
            xmax = p2.x()
            xmin = p1.x()
        if p2.y() < p1.y():
            ymin = p2.y()
            ymax = p1.y()
        else:
            ymax = p2.y()
            ymin = p1.y()
        self.update(QRect(QPoint(xmin - self.pen_width, ymin - self.pen_width),
                          QPoint(xmax + self.pen_width, ymax + self.pen_width)))

    def show_scribble_area(self):
        self.view.hide()
        self.current_scene.clearSelection()
        self.sync_sketch_view()
        self.update()

    def show_sketch_view(self):
        self.view.show()
