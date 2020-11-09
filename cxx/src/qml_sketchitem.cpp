#include "qml_sketchitem.hpp"

using namespace std;

SketchItem::SketchItem(QQuickItem *parent)
        : QQuickPaintedItem(parent), bufPainter(nullptr) {
    qDebug() << "SketchItem::SketchItem";
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptTouchEvents(true);
    pen.setColor(Qt::white);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
}

void SketchItem::setPenWidth(const int &_penWidth) {
    qDebug() << "setPenWidth";
    pen.setWidth(_penWidth);
    if (bufPainter) {
        bufPainter->setPen(pen);
    }
}

void SketchItem::setPenColor(const QColor &_penColor) {
    qDebug() << "setPenColor";
    pen.setColor(_penColor);
    if (bufPainter) {
        bufPainter->setPen(pen);
    }
}

void SketchItem::invokePinchStarted(const qreal &_angle, const qreal &_scale) {
    qInfo() << _angle << _scale;
}

void SketchItem::invokePinchUpdated(const qreal &_angle, const qreal &_scale) {
    qInfo() << _angle << _scale;
}

void SketchItem::invokePinchFinished(const qreal &_angle, const qreal &_scale) {
    qInfo() << _angle << _scale;
}

void SketchItem::invokePressed(const qreal &_x, const qreal &_y) {
    qInfo() << QPointF(_x, _y);
}

void SketchItem::invokePositionChanged(const qreal &_x, const qreal &_y) {
    qInfo() << QPointF(_x, _y);
}

void SketchItem::invokeReleased(const qreal &_x, const qreal &_y) {
    qInfo() << QPointF(_x, _y);
}

void SketchItem::paint(QPainter *painter) {
    qDebug() << "SketchItem::paint";
    for (auto &imgLayer : imgLayers) {
        if (!imgLayer.second) {
            continue;
        }
        auto rect = imgLayer.first.rect();
        painter->drawPixmap(rect, imgLayer.first, rect);
    }
}

void SketchItem::init(const int &_w, const int &_h) {
    qDebug() << "SketchItem::init()";
    imgLayers.clear();
    imgLayers.push_back(qMakePair(QPixmap(_w, _h), true));
    imgLayers.begin()->first.fill(Qt::darkGray);
    bufPainter = make_shared<QPainter>(&(imgLayers.begin()->first));
    bufPainter->setPen(pen);
    bufPainter->setRenderHint(QPainter::Antialiasing);
}

void SketchItem::mousePressEvent(QMouseEvent *event) {
    //  qDebug() << "SketchItem::mousePress@" << event->pos();
    lastPos = event->localPos();

    putPointAt(lastPos);
}

void SketchItem::putLineAt(const QPointF &_p1, const QPointF &_p2) {
    bufPainter->drawLine(_p1, _p2);
    qreal pad2x = pen.widthF() * 2;
    qreal pad4x = pad2x * 2;
    int x = floor((min)(_p1.x(), _p2.x()) - pad2x);
    int y = floor((min)(_p1.y(), _p2.y()) - pad2x);
    int w = ceil(abs(_p1.x() - _p2.x()) + pad4x);
    int h = ceil(abs(_p1.y() - _p2.y()) + pad4x);
    update(QRect(x, y, w, h));
}

void SketchItem::putPointAt(const QPointF &_p) {
    bufPainter->drawPoint(_p);
    qreal pad2x = pen.widthF() * 2;
    qreal pad4x = pad2x * 2;
    int x, y, w, h;
    x = y = floor(_p.x() - pad2x);
    w = h = ceil(pad4x);
    update(QRect(x, y, w, h));
}

void SketchItem::mouseMoveEvent(QMouseEvent *event) {
    //  qDebug() << "SketchItem::mouseMove@" << event->pos();
    auto curPos = event->localPos();
    putLineAt(curPos, lastPos);
    lastPos = curPos;
}

void SketchItem::mouseReleaseEvent(QMouseEvent *event) {
    //  qDebug() << "SketchItem::mouseRelease@" << event->pos();
}
