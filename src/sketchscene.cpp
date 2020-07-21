#include "sketchscene.h"
#include <QGraphicsSceneMouseEvent>

SketchScene::SketchScene(QObject *parent) : QGraphicsScene(parent) {

}

SketchScene::SketchScene(const QRectF &sceneRect, QObject *parent)
        : QGraphicsScene(sceneRect, parent) {
}

SketchScene::SketchScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
        : QGraphicsScene(x, y, width, height, parent) {
}

SketchScene::~SketchScene() {
}

void SketchScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mousePressEvent(event);
//    qDebug()<<"press..."<<event->scenePos();


}

void SketchScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseMoveEvent(event);
//    qDebug()<<"move..."<<event->scenePos();
}

void SketchScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseReleaseEvent(event);
//    qDebug()<<"release...";
}

QGraphicsPathItem *SketchScene::commitPath() {
    return addPath(currentPath);
}

void SketchScene::addPathMoveTo(const QPointF &p) {
    currentPath.moveTo(p);
}

void SketchScene::addPathLineTo(const QPointF &p) {
    currentPath.lineTo(p);
}

void SketchScene::clearPath() {
    currentPath.clear();
}

