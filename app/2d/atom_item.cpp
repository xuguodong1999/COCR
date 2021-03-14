#include "atom_item.hpp"


AtomItem::AtomItem(QGraphicsItem *parent) : BaseItem(parent) {
    textItem = new QGraphicsTextItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void AtomItem::setHTML(const QString &_html) {
    textItem->setHtml(_html);
}

void AtomItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    textItem->paint(painter, option, widget);
}

QRectF AtomItem::boundingRect() const {
    return textItem->boundingRect();
}

void AtomItem::setPos2D(const float &_x, const float &_y) {
    auto rect = boundingRect();
    setPos({_x - rect.x() - rect.width() / 2, _y - rect.y() - rect.height() / 2});
}
