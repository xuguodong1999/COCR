#include "switchbutton.h"
#include <QPainterPath>

SwitchButton::SwitchButton(QWidget *parent) : QAbstractButton(parent) {
    setCheckable(true);
    setAnimationEnabled(true);
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &SwitchButton::updateValue);
    setChecked(false);
}

SwitchButton::~SwitchButton() {
}

void SwitchButton::paintEvent(QPaintEvent *) {
//    qDebug() << "SwitchButton::paintEvent";
    QPainter painter(this);
    drawBackGround(painter);
    drawSlider(painter);
}

void SwitchButton::mousePressEvent(QMouseEvent *e) {
    QAbstractButton::mousePressEvent(e);
    mStep = width() / 100;
    if (isChecked()) {
        mEndX = 0;
        mStartX = width() - height();
    } else {
        mEndX = width() - height();
        mStartX = 0;
    }

    if (animationEnabled) {
        mTimer->setInterval(1);
        mTimer->start();
    } else {
        mStartX = mEndX;
        update();
    }
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *e) {
    QAbstractButton::mouseReleaseEvent(e);
}

void SwitchButton::updateValue() {
    if (isChecked()) {
        if (mStartX > mEndX) {
            mStartX -= mStep;
        } else {
            mStartX = mEndX;
            mTimer->stop();
        }

    } else {
        if (mStartX < mEndX) {
            mStartX += mStep;
        } else {
            mStartX = mEndX;
            mTimer->stop();
        }
    }
    update();
}

void SwitchButton::drawSlider(QPainter &painter) {
    painter.save();
    const auto &roundButtonColor = isChecked() ? mRoundButtonColorOff : mRoundButtonColorOn;
    painter.setBrush(QBrush(roundButtonColor));
    painter.setPen(Qt::NoPen);
    int sliderWidth = qMin(width(), height()) - m_space * 2;
    QRect rect(m_space + mStartX, m_space, sliderWidth, sliderWidth);
    painter.drawEllipse(rect);
    QPainterPath outline;
    outline.addEllipse(rect);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawPath(outline);
    painter.restore();
}

void SwitchButton::drawBackGround(QPainter &painter) {
    painter.save();
    painter.setPen(Qt::NoPen);
    QColor bgColor = isChecked() ? mBgColorOff : mBgColorOn;
    if (isEnabled()) {
        bgColor.setAlpha(127);
    }
    painter.setBrush(bgColor);
    QRect rect(0, 0, width(), height());
    int boxSize = qMin(width(), height());
    //左侧圆
    QPainterPath roundLeftPath;
    roundLeftPath.addEllipse(rect.x(), rect.y(), boxSize, boxSize);
    //右侧圆
    QPainterPath roundRightPath;
    roundRightPath.addEllipse(rect.width() - boxSize, rect.y(), boxSize, boxSize);
    //中间矩形
    QPainterPath rectPath;
    rectPath.addRect(rect.x() + boxSize / 2, rect.y(), rect.width() - boxSize, height());
    QPainterPath path = roundLeftPath + roundRightPath + rectPath;
    painter.drawPath(path);
    //绘制文本
    int roundWidth = boxSize - m_space * 2;
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(QPen(mTextColor));
    if (isChecked()) {// 从左往右滑动
        painter.drawText(QRect(0, 0, width() - roundWidth, height()),
                         Qt::AlignCenter, mTextOff);
    } else {
        painter.drawText(QRect(roundWidth, 0, width() - roundWidth, height()),
                         Qt::AlignCenter, mTextOn);
    }
    painter.restore();
}

bool SwitchButton::isAnimationEnabled() const {
    return animationEnabled;
}

void SwitchButton::setAnimationEnabled(bool animationEnabled) {
    SwitchButton::animationEnabled = animationEnabled;
}

void SwitchButton::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    m_space = qMin(2, qMin(width(), height()) / 10);
    if (isChecked()) {
        mEndX = 0;
        mStartX = width() - height();
    } else {
        mEndX = width() - height();
        mStartX = 0;
    }
}
