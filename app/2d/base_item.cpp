#include "base_item.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BaseItem::BaseItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
}

void BaseItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    QGraphicsItem::mousePressEvent(e);
}

void BaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
    QGraphicsItem::mouseMoveEvent(e);
    emit sig_mouse_move(e->scenePos());
}

void BaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    QGraphicsItem::mouseReleaseEvent(e);
}
