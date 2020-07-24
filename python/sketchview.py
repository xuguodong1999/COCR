from PySide2.QtCore import Qt, Signal
from PySide2.QtGui import QCursor
from PySide2.QtWidgets import QGraphicsView


class SketchView(QGraphicsView):
    def __init__(self):
        super(SketchView, self).__init__()

    def focusOutEvent(self, event):
        super(SketchView, self).focusOutEvent(event)
        cursor = QCursor()
        cursor.setShape(Qt.CustomCursor)
        self.setCursor(cursor)
        self.hideFromTop.emit()

    def focusInEvent(self, event):
        super(SketchView, self).focusInEvent(event)
        cursor = QCursor()
        cursor.setShape(Qt.OpenHandCursor)
        self.setCursor(cursor)
        self.showAtTop.emit()

    def mousePressEvent(self, event):
        super(SketchView, self).mousePressEvent(event)
        cursor = QCursor()
        cursor.setShape(Qt.ClosedHandCursor)
        self.setCursor(cursor)

    def mouseReleaseEvent(self, event):
        super(SketchView, self).mouseReleaseEvent(event)
        cursor = QCursor()
        cursor.setShape(Qt.OpenHandCursor)
        self.setCursor(cursor)

    @Signal
    def hideFromTop(self):
        pass

    @Signal
    def showAtTop(self):
        pass
