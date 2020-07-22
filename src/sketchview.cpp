#include "sketchview.h"

SketchView::SketchView(QWidget *parent)
        : QGraphicsView(parent) {
}

SketchView::SketchView(QGraphicsScene *scene, QWidget *parent)
        : QGraphicsView(scene, parent) {
}

SketchView::~SketchView() {
}

void SketchView::focusOutEvent(QFocusEvent *event) {
    QGraphicsView::focusOutEvent(event);
    QCursor cursor;
    cursor.setShape(Qt::CustomCursor);
    setCursor(cursor);
    emit hideFromTop();
}

void SketchView::focusInEvent(QFocusEvent *event) {
    QGraphicsView::focusInEvent(event);
    QCursor cursor;
    cursor.setShape(Qt::OpenHandCursor);
    setCursor(cursor);
    emit showAtTop();
}

void SketchView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
    QCursor cursor;
    cursor.setShape(Qt::ClosedHandCursor);
    setCursor(cursor);
}

void SketchView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    QCursor cursor;
    cursor.setShape(Qt::OpenHandCursor);
    setCursor(cursor);
}



