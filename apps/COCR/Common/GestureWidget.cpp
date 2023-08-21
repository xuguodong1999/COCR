#include "GestureWidget.h"
#include <QDebug>

GestureWidget::GestureWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);
    qDebug() << "GestureWidget::GestureWidget()";
}

bool GestureWidget::event(QEvent *e) {
    switch (e->type()) {
        case QEvent::Gesture:
            return gestureEvent(static_cast<QGestureEvent *>(e));
        default:
            return QWidget::event(e);
    }
}

void GestureWidget::zoom(const float &_k) {

}

void GestureWidget::translate(const QPointF &_dir) {

}

void GestureWidget::rotate(const float &_k) {

}

bool GestureWidget::gestureEvent(QGestureEvent *e) {
    QGesture *pinchGesture = e->gesture(Qt::PinchGesture);
    if (pinchGesture) {
        onPinchGesture(static_cast<QPinchGesture *>(pinchGesture));
    }
    return true;
}

void GestureWidget::onPinchGesture(QPinchGesture *gesture) {
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        zoom(gesture->totalScaleFactor());
        rotate(gesture->rotationAngle());
    }
    if (gesture->state() == Qt::GestureFinished) {
        // TODO:
    }

}

