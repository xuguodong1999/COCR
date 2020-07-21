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
    emit hideFromTop();
}

void SketchView::focusInEvent(QFocusEvent *event) {
    QGraphicsView::focusInEvent(event);
    emit showAtTop();
}



