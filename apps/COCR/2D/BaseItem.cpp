#include "BaseItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BaseItem::BaseItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
}

void BaseItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    isPressed = true;
    QGraphicsItem::mousePressEvent(e);
}

void BaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
    Q_EMIT sig_mouse_move(e->scenePos());
    QGraphicsItem::mouseMoveEvent(e);
}

void BaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    if (isPressed) {
        Q_EMIT sig_mouse_click(e->scenePos());
    }
    isPressed = false;
    QGraphicsItem::mouseReleaseEvent(e);
}

void BaseItem::SetView2DWidget(View2DWidget *_view2DWidget) {
    view2DWidget = _view2DWidget;
}
