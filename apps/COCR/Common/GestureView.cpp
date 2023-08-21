#include "GestureView.h"

#include <QDebug>
#include <QGuiApplication>

GestureView::GestureView(QWidget *parent) : QGraphicsView(parent), curAngle(0), curX(0), curY(0), curScale(1),
                                            sumAngle(0), sumScale(1) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);
    qDebug() << "GestureView::GestureView";
}


bool GestureView::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::Gesture:
            return gestureEvent(static_cast<QGestureEvent *>(event));
        default:
            return QGraphicsView::event(event);
    }
}

void GestureView::zoom(const float &_k) {
//    qDebug() << "GestureView::zoom" << _k;
    QGraphicsView::scale(_k, _k);
    sumScale *= _k;
}

void GestureView::translate(const QPointF &_dir) {
//    qDebug() << "GestureView::translate" << _dir;
    QGraphicsView::translate(_dir.x(), _dir.y());
}

void GestureView::rotate(const float &_k) {
//    qDebug() << "GestureView::rotate" << _k;
    QGraphicsView::rotate(_k);
    sumAngle += _k;
}

bool GestureView::gestureEvent(QGestureEvent *event) {
    QGesture *pinchGesture = event->gesture(Qt::PinchGesture);
    if (pinchGesture) {
        onPinchGesture(static_cast<QPinchGesture *>(pinchGesture));
    }
    return true;
}

void GestureView::onPinchGesture(QPinchGesture *gesture) {
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        zoom(gesture->totalScaleFactor() / curScale);
        curScale = gesture->totalScaleFactor();
        rotate(gesture->rotationAngle() - curAngle);
        curAngle = gesture->rotationAngle();

    }
    if (gesture->state() == Qt::GestureFinished) {
        curAngle = 0;
        curScale = 1;
    }

}

void GestureView::wheelEvent(QWheelEvent *event) {
    float k = 0.05;
    bool up = event->angleDelta().y() > 0;
    if (qApp->keyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
        k = 0.2;
        if (up) { zoom(1 + k); }
        else { zoom(1 - k); }
    } else if (qApp->keyboardModifiers() == Qt::ControlModifier) {
        if (up) { zoom(1 + k); }
        else { zoom(1 - k); }
    } else {
        if (up) { rotate(5); }
        else { rotate(-5); }
    }
    QGraphicsView::wheelEvent(event);
}


void GestureView::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            reset();
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            translate({0, -sTransStep});
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            translate({-sTransStep, 0});
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            translate({0, sTransStep});
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            translate({sTransStep, 0});
            break;
        default:
            break;
    }
    QGraphicsView::keyReleaseEvent(event);
}

void GestureView::reset() {
    curScale = 1;
    curAngle = curX = curY = 0;
//    translate({curX, curY});
//    rotate(curAngle);
//    zoom(curScale);
    QGraphicsView::centerOn(0, 0);
    QGraphicsView::rotate(-sumAngle);
    QGraphicsView::scale(1.f / sumScale, 1.f / sumScale);
    setSceneRect(QRectF(0, 0, width(), height()));
    sumAngle = 0;
    sumScale = 1;
}

void GestureView::mouseDoubleClickEvent(QMouseEvent *e) {
    reset();
    QGraphicsView::mouseDoubleClickEvent(e);
}

