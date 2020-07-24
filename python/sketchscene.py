from PySide2.QtCore import Qt
from PySide2.QtGui import QPainterPath, QPixmap
from PySide2.QtWidgets import QGraphicsScene


class SketchScene(QGraphicsScene):
    def __init__(self):
        super(SketchScene, self).__init__()
        self.current_path = QPainterPath()
        self.bg_pixmap_item = None

    def mousePressEvent(self, event):
        super(SketchScene, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        super(SketchScene, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        super(SketchScene, self).mouseReleaseEvent(event)

    def commitPath(self):
        return self.addPath(self.current_path)

    def addPathMoveTo(self, p):
        self.current_path.moveTo(p)

    def addPathLineTo(self, p):
        self.current_path.lineTo(p)

    def clearPath(self):
        self.current_path = QPainterPath()

    def addBackGround(self, size):
        if self.bg_pixmap_item is not None:
            if self.bg_pixmap_item.pixmap().size() == size:
                return
            self.removeItem(self.bg_pixmap_item)
        tmp = QPixmap(size)
        tmp.fill(Qt.white)
        self.bg_pixmap_item = self.addPixmap(tmp)
        self.bg_pixmap_item.setZValue(-1)
        self.bg_pixmap_item.setActive(False)

    def clearAll(self):
        self.clear()
        self.bg_pixmap_item = None
