#include "atom_item.hpp"


AtomItem::AtomItem(QGraphicsItem *parent) : BaseItem(parent) {
    mTextItem = new QGraphicsTextItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    connect(this, &BaseItem::sig_mouse_move, [&](const QPointF &) {
        auto r = boundingRect();
        auto p = pos();
        mCenter = {p.x() + r.width() / 2, p.y() + r.height() / 2};
    });
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

void AtomItem::setPos2D(const qreal &_x, const qreal &_y) {
    mCenter = {_x, _y};
    auto rect = boundingRect();
    setPos({_x - rect.x() - rect.width() / 2, _y - rect.y() - rect.height() / 2});
}

float AtomItem::GetCommonSize() {
    return sCommonSize;
}
