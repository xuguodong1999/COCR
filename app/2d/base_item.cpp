#include "base_item.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BaseItem::BaseItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
}

void BaseItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    isPressed = true;
    QGraphicsItem::mousePressEvent(e);
}

void BaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
    emit sig_mouse_move(e->scenePos());
    QGraphicsItem::mouseMoveEvent(e);
}

void BaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    if (isPressed) {
        emit sig_mouse_click(e->scenePos());
    }
    isPressed = false;
    QGraphicsItem::mouseReleaseEvent(e);
}
