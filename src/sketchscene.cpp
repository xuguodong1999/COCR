#include "sketchscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QGraphicsPixmapItem>
#include <QPainterPath>

SketchScene::SketchScene(QObject *parent)
        : QGraphicsScene(parent), bgPixmapItem(nullptr) {

}

SketchScene::SketchScene(const QRectF &sceneRect, QObject *parent)
        : QGraphicsScene(sceneRect, parent), bgPixmapItem(nullptr) {
}

SketchScene::SketchScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
        : QGraphicsScene(x, y, width, height, parent), bgPixmapItem(nullptr) {
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
//    currentPath.clear();
    currentPath = QPainterPath();
}

void SketchScene::addBackGround(const QSize &size) {
    if (bgPixmapItem != nullptr) {
        if (bgPixmapItem->pixmap().size() == size) {
            return;
        }
        removeItem(reinterpret_cast<QGraphicsItem *>(bgPixmapItem));
    }
    QPixmap tmp(size);
    tmp.fill(Qt::white);
    bgPixmapItem = addPixmap(tmp);
    bgPixmapItem->setZValue(-1);
    bgPixmapItem->setActive(false);
}

void SketchScene::clearAll() {
    clear();
    bgPixmapItem = nullptr;
}

