from PySide2.QtCore import QTimer, Qt, Slot, QRect
from PySide2.QtGui import QColor, QPainter, QPainterPath, QBrush, QPen, QFont
from PySide2.QtWidgets import QAbstractButton


class SwitchButton(QAbstractButton):
    def __init__(self):
        super(SwitchButton, self).__init__()
        self.setCheckable(True)
        self.animationEnabled = True
        self.mTimer = QTimer(self)
        self.mTimer.timeout.connect(self.updateValue)
        self.setChecked(False)
        self.mStep = 0
        self.mEndX = 0
        self.mStartX = 0
        self.m_space = 0
        self.mRoundButtonColorOn = QColor(Qt.white)
        self.mRoundButtonColorOff = QColor(Qt.white)
        self.mBGColorOn = QColor(Qt.lightGray)
        self.mBgColorOff = QColor(Qt.darkGray)
        self.mTextColor = QColor(Qt.black)
        self.mTextOn = "绘制"
        self.mTextOff = "拖动"

    def paintEvent(self, event):
        # super(SwitchButton, self).paintEvent(event)
        painter = QPainter(self)
        self.drawBackGround(painter)
        self.drawSlider(painter)

    def mousePressEvent(self, event):
        super(SwitchButton, self).mousePressEvent(event)
        self.mStep = self.width() / 100
        if self.isChecked():
            self.mEndX = 0
            mStartX = self.width() - self.height()
        else:
            self.mEndX = self.width() - self.height()
            self.mStartX = 0
        if self.animationEnabled:
            self.mTimer.setInterval(1)
            self.mTimer.start()
        else:
            self.mStartX = self.mEndX
            self.update()

    def mouseReleaseEvent(self, event):
        super(SwitchButton, self).mouseReleaseEvent(event)

    def resizeEvent(self, event):
        super(SwitchButton, self).resizeEvent(event)
        m_space = min(2, min(self.width(), self.height()) / 10)
        if self.isChecked():
            self.mEndX = 0
            self.mStartX = self.width() - self.height()
        else:
            self.mEndX = self.width() - self.height()
            self.mStartX = 0
        self.update()

    def isAnimationEnabled(self):
        return self.animationEnabled

    def setAnimationEnabled(self, animationEnabled):
        self.animationEnabled = animationEnabled

    @Slot()
    def updateValue(self):
        if self.isChecked():
            if self.mStartX > self.mEndX:
                self.mStartX -= self.mStep
            else:
                self.mStartX = self.mEndX
                self.mTimer.stop()
        else:
            if self.mStartX < self.mEndX:
                self.mStartX += self.mStep
            else:
                self.mStartX = self.mEndX
                self.mTimer.stop()
        self.update()

    def drawSlider(self, painter):
        painter.save()
        if self.isChecked():
            roundButtonColor = self.mRoundButtonColorOff
        else:
            roundButtonColor = self.mRoundButtonColorOn
        painter.setBrush(QBrush(roundButtonColor))
        painter.setPen(Qt.NoPen)
        sliderWidth = min(self.width(), self.height()) - self.m_space * 2
        rect = QRect(self.m_space + self.mStartX, self.m_space, sliderWidth, sliderWidth)
        painter.drawEllipse(rect)
        outline = QPainterPath()
        outline.addEllipse(rect)
        painter.setPen(QPen(Qt.black, 2))
        painter.drawPath(outline)
        painter.restore()

    def drawBackGround(self, painter):
        painter.save()
        painter.setPen(Qt.NoPen)
        if self.isChecked():
            bgColor = self.mBgColorOff
        else:
            bgColor = self.mBGColorOn
        if self.isEnabled():
            bgColor.setAlpha(127)
        painter.setBrush(bgColor)
        rect = QRect(0, 0, self.width(), self.height())
        boxSize = min(self.width(), self.height())
        # 左侧圆
        roundLeftPath = QPainterPath()
        roundLeftPath.addEllipse(rect.x(), rect.y(), boxSize, boxSize)
        # 右侧圆
        roundRightPath = QPainterPath()
        roundRightPath.addEllipse(rect.width() - boxSize, rect.y(), boxSize, boxSize)
        # 中间矩形
        rectPath = QPainterPath()
        rectPath.addRect(rect.x() + boxSize / 2, rect.y(), rect.width() - boxSize, self.height())
        path = roundLeftPath + roundRightPath + rectPath
        painter.drawPath(path)
        # 绘制文本
        roundWidth = boxSize - self.m_space * 2
        painter.setFont(QFont("Arial", 12, QFont.Bold))
        painter.setPen(QPen(self.mTextColor))
        if self.isChecked():  # 从左往右滑动
            painter.drawText(QRect(0, 0, self.width() - roundWidth, self.height()), Qt.AlignCenter, self.mTextOff)
        else:
            painter.drawText(QRect(roundWidth, 0, self.width() - roundWidth, self.height()), Qt.AlignCenter,
                             self.mTextOn)
        painter.restore()
