#include "atom_item.hpp"


AtomItem::AtomItem(QGraphicsItem *parent) : BaseItem(parent) {
    mTextItem = new QGraphicsTextItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void AtomItem::setHTML(const QString &_html) {
    mTextItem->setHtml(_html);
}

void AtomItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    mTextItem->paint(painter, option, widget);
}

QRectF AtomItem::boundingRect() const {
    return mTextItem->boundingRect();
}

void AtomItem::setLeftTop(const qreal &_x, const qreal &_y) {
    setPos(_x, _y);
}

float AtomItem::GetCommonSize() {
    return sCommonSize;
}
