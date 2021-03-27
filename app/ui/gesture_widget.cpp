#include "gesture_widget.hpp"
#include <QDebug>

GestureWidget::GestureWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);
    qDebug() << "GestureWidget::GestureWidget()";
}

bool GestureWidget::event(QEvent *e) {
    qDebug() << "GestureWidget::event" << e->type();
    switch (e->type()) {
        case QEvent::Gesture:
            return gestureEvent(static_cast<QGestureEvent *>(e));
        default:
            return QWidget::event(e);
    }
}

void GestureWidget::zoom(const float &_k) {
    qDebug() << "zoom:" << _k;
}

void GestureWidget::translateBy(const QPointF &_dir) {

}

void GestureWidget::rotate(const float &_k) {
    qDebug() << "rotate:" << _k;
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

//////////////////////////////////

GestureView::GestureView(QWidget *parent) : QGraphicsView(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);
    qDebug() << "GestureView::GestureView";
}


bool GestureView::event(QEvent *event) {
    qDebug() << event->type();
    switch (event->type()) {
        case QEvent::Gesture:
            return gestureEvent(static_cast<QGestureEvent *>(event));
        default:
            return QGraphicsView::event(event);
    }
}

void GestureView::zoom(const float &_k) {
    qDebug() << "zoom:" << _k;
}

void GestureView::translateBy(const QPointF &_dir) {

}

void GestureView::rotate(const float &_k) {
    qDebug() << "rotate:" << _k;
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
        zoom(gesture->totalScaleFactor());
        rotate(gesture->rotationAngle());
    }
    if (gesture->state() == Qt::GestureFinished) {
        // TODO:
    }

}